import numpy
import scipy.fftpack
from .Segment import Segment
from .UtilityFunctions import angleOfVectors

# Trip class
class Trip:
    
    # Constructor
    def __init__( self, tripId ):
        self.__lastPoint = None
        self.__segments = []
        self.__id = tripId
        self.__segmentsGenerated = False
        self.__extraTravelDuration = 0
        self.__extraTravelLength = 0
        return

    def __del__( self ):
        return

    # Returns the trip id
    def id( self ):
        return self.__id

    # Sets directly the raw data from a numpy array
    def setData( self, data ):
        self.__lastPoint = data[ len(data)-1]
        self.__tripDuration = len(data)-1
        self.__extraTravelDuration = 0
        self.__extraTravelLength = 0
        self.__generateSegments( data )
        return
    
    def attributes( self ):
        outputValues = []
        outputLabels = []
        
        # Zero segment flag
        outputLabels.append('ZeroSegments')
        zeroSegments = 0
        if len( self.__segments ) == 0 : zeroSegments = 1
        outputValues.append( zeroSegments )
        
        # Few points flag
        outputLabels.append( 'FewPoints' )
        fewPoints = 0
        if numpy.sum([ s.duration() + 1 for s in self.__segments ]) < 20: fewPoints = 1
        outputValues.append( fewPoints )
        
        # Trip Duration
        outputLabels.append( 'TripDuration' )
        outputValues.append( self.tripDuration() )
        
        # Trip Length
        outputLabels.append( 'TripLength' )
        tripLength = self.distanceTraveled()
        outputValues.append( tripLength )
        
        # Distance to Travel
        outputLabels.append( 'TravelToDistance' )
        outputValues.append( tripLength / self.distanceToEndPoint() )
        
        # Speed percentiles
        percentiles = [25,50,75,95]
        percentileValues = self.speedStatistics( percentiles )
        for i in range(len(percentiles)):
            label = 'speedP'
            if percentiles[i] < 10: label += '0'
            outputLabels.append( label + str(percentiles[i]) )
            outputValues.append( percentileValues[i] )
        
        # Acceleration percentiles
        percentiles = [5,25,75,95]
        percentileValues = self.accelerationStatistics( percentiles )
        for i in range(len(percentiles)):
            label = 'accelerationP'
            if percentiles[i] < 10: label += '0'
            outputLabels.append( label + str(percentiles[i]) )
            outputValues.append( percentileValues[i] )
            
        # Direction percentiles
        percentiles = [5,95]
        percentileValues = self.directionStatistics( percentiles )
        for i in range(len(percentiles)):
            label = 'directionP'
            if percentiles[i] < 10: label += '0'
            outputLabels.append( label + str(percentiles[i]) )
            outputValues.append( percentileValues[i] )
            
        # Speed x Acceleration
        percentiles = [5,25,75,95]
        percentileValues = self.speedTimesAccelerationStatistics( percentiles )
        for i in range(len(percentiles)):
            label = 'speedXaccelerationP'
            if percentiles[i] < 10: label += '0'
            outputLabels.append( label + str(percentiles[i]) )
            outputValues.append( percentileValues[i] )
            
        # Total direction change
        outputLabels.append('totalDirectionChange')
        outputValues.append( self.totalDirectionChange( 0.035 ) ) # 2 degree threshold
        
        # The rolling FFT of the speed values
        sampleSize = 10
        transformationValues = self.rollingFFT( sampleSize )
        for i in range(len(transformationValues)):
            outputLabels.append( 'fft' + str(i) )
            outputValues.append( transformationValues[i] )
        
        return numpy.array(outputValues), outputLabels

    
    # The time duration of the trip
    def tripDuration( self ):
        result = 0
        for segment in self.__segments:
            result += segment.duration()        
        return result + self.__extraTravelDuration

    
    # The trip segments
    def segments( self ):
        return self.__segments

    
    # Returns the number of segments
    def numberOfSegments( self ):
        return len(self.__segments)

    
    # Returns the distance to the ending point
    def distanceToEndPoint( self ):
        return numpy.sqrt( self.__lastPoint[0]**2 + self.__lastPoint[1]**2 )

    
    # The total distance traveled, derived from the segments
    def distanceTraveled( self ):
        result = 0
        for segment in self.__segments:
            result += segment.distanceTraveled()
        return result + self.__extraTravelLength

    
    # The mean spead derived by the segments
    def meanSpeed( self ):
        dS = 0
        dt = 0
        for segment in self.__segments:
            dS += segment.distanceTraveled()
            dt += segment.duration()
        if dt == 0:
            return 0
        else:
            return dS/dt
        
        
    # The total direction change
    def totalDirectionChange( self, directionNoiseThreshold ):
        values = self.directionValues()
        result = 0
        for value in values:
            direction = numpy.abs(value)
            if direction > directionNoiseThreshold:
                result += direction
        travelLength = self.distanceTraveled()
        if travelLength == 0: return numpy.nan
        else:
            return result / travelLength

        
    # The speed values
    def speedValues( self ):
        speedValues = numpy.array([])
        for segment in self.__segments:
            speedValues = numpy.hstack( (speedValues, segment.speedValues() ) )
        return speedValues
    

    # The speed values distribution
    def speedStatistics( self, percentiles ):
        speedValues = self.speedValues()
        if len( speedValues ) < 3:
            result = numpy.empty(len(percentiles))
            result[:] = numpy.nan
            return result
        else:
            return numpy.percentile( speedValues, percentiles )

        
    # The acceleration values
    def accelerationValues( self ):
        accelerationValues = numpy.array([])
        for segment in self.__segments:
            accelerationValues = numpy.hstack( (accelerationValues, segment.accelerationValues() ) )
        return accelerationValues

    
    # The acceleration values distribution
    def accelerationStatistics( self, percentiles ):
        accelerationValues = self.accelerationValues()
        if len( accelerationValues ) < 3:
            result = numpy.empty(len(percentiles))
            result[:] = numpy.nan
            return result
        else:
            return numpy.percentile(accelerationValues, percentiles )

        
    # The direction angles
    def directionValues( self ):
        angularValues = numpy.array([])
        for segment in self.__segments:
            angularValues = numpy.hstack( (angularValues, segment.angularValues() ) )
        return angularValues
        

    # The direction angles distribution
    def directionStatistics( self, percentiles ):
        angularValues = self.directionValues()
        if len( angularValues ) < 3:
            result = numpy.empty(len(percentiles))
            result[:] = numpy.nan
            return result
        else:
            return numpy.percentile(angularValues, percentiles )
        
        
    # The speed times acceleration statistics
    def speedTimesAccelerationStatistics( self, percentiles ):
        values = numpy.array([])
        for segment in self.__segments:
            speedValues = segment.speedValues()
            accelerationValues = segment.accelerationValues()
            for i in range(len(accelerationValues) ):
                values = numpy.hstack( (values, speedValues[i] * accelerationValues[i]) )
        if len(values) < 3:
            result = numpy.empty(len(percentiles))
            result[:] = numpy.nan
            return result
        else:
            return numpy.percentile(values, percentiles)

    
    # Rolling FFT of the speed values
    def rollingFFT( self, sampleSize ):
        numberOfTransformations = 0
        transformationSize = int( numpy.floor( (sampleSize - 1 ) / 2 ) ) + (sampleSize+1)%2;
        result = numpy.zeros( transformationSize )
        for segment in self.__segments:
            segmentValues = segment.speedValues()
            startingIndex = 0
            endIndex = sampleSize
            while endIndex <= len(segmentValues):
                sample = segmentValues[startingIndex : endIndex ]
                transformed = numpy.abs( scipy.fftpack.fft( sample ) )
                power = numpy.zeros( transformationSize )
                for i in range(transformationSize):
                    power[i] = numpy.abs( transformed[i] )
                result += power
                numberOfTransformations += 1
                startingIndex += 1
                endIndex += 1
        if numberOfTransformations > 0:
            return result / numberOfTransformations
        else:
            result[:] = numpy.nan
            return result
        
    
    # Private function that generates the trip segments
    def __generateSegments(self, rawData ):
        if self.__segmentsGenerated :
            return
        
        self.__extraTravelDuration = 0
        self.__extraTravelLength = 0
        
        # First pass : remove the zero speed segments
        segments = self.__removeZeroSpeedSegments( rawData )

        # Second pass : identify gaps and correct speed jitter
        segments2 = []
        for segment in segments:
            subsegments = self.__identifyGapsCorrectJitter( segment )
            for subsegment in subsegments: segments2.append( subsegment )

        # Third pass : remove angular jitter
        segments = []
        for segment in segments2:
            subsegments = self.__removeAccuteAngleSegments( segment )
            for subsegment in subsegments: segments.append( subsegment )
        
        # Fourth pass : treat the gap and the jitters again
        segments2 = []
        for segment in segments:
            subsegments = self.__identifyGapsCorrectJitter( segment )
            for subsegment in subsegments: segments2.append( subsegment )        

        # Create the segment objects
        segments = segments2
        
        self.__segments = []
        for segment in segments:
            self.__segments.append( Segment( segment ) )
        
        self.__segmentsGenerated = True
        return

    # Function to apply the removal of the zero speed segments
    def __removeZeroSpeedSegments( self, tripData ):
        zeroSpeedTolerance = 1.5

        segments=[]
        segentStartingIndex = 0
        zeroSpeedCounter = 0
        p_previous = tripData[0]
        for i in range(1,len(tripData)):
            p_current = tripData[i]
            v_current = p_current - p_previous
            v_current = numpy.sqrt(v_current[0]**2 + v_current[1]**2)

            if v_current < zeroSpeedTolerance:
                if zeroSpeedCounter == 0 : # Mark the end of a segment and beginning of a zero speed sequence
                    if i - segentStartingIndex > 1:
                        segments.append( tripData[ segentStartingIndex : i] )
                zeroSpeedCounter += 1
                segentStartingIndex = i
            else:
                zeroSpeedCounter = 0

            p_previous = p_current

        if len(tripData) - segentStartingIndex > 2:
            segments.append( tripData[segentStartingIndex : len(tripData)] )

        return segments
    
    # Function to identify the gaps and corrects for the speed jitter
    def __identifyGapsCorrectJitter( self, tripData ):
        maxAcceleration = 5
        speedToTrigger = 10
        segments = []
        
        v_previous = 0
        p_previous = tripData[0]
        i = 1
        segmentStartingIndex = 0
        
        while i < len(tripData) :
            p_current = tripData[i]
            v_current = numpy.linalg.norm( p_current - p_previous )
            
            # Check for abrupt high speed and high acceleration in combination with no low speed
            if ( numpy.abs( v_current - v_previous ) > maxAcceleration ) & ( v_current > speedToTrigger ):
                # Signal the end of the previous segment
                nDiff = i - segmentStartingIndex
                if nDiff > 1:
                    segments.append( tripData[ segmentStartingIndex : i ] )
                else:
                    # Correct trip duration and length for the skipped mini-segment
                    if ( i < len(tripData) - 1 ):
                        p_next = tripData[i]
                        v_next = numpy.linalg.norm( p_next - p_current )
                        if numpy.abs( v_current - v_next ) < maxAcceleration:
                            self.__extraTravelDuration += 1
                            self.__extraTravelLength += numpy.lingalg.norm( p_next - p_previous )
                
                # Check whether this is a jitter or a gap and adjust the starting index accordingly, as well as the correction to the travel length and time.
                if i < len(tripData) -1 :
                    p_next = tripData[i+1]
                    v_next = numpy.linalg.norm( p_next - p_current )
                    if numpy.abs( v_current - v_next ) > maxAcceleration:
                        averageSpeedInGap = 0.5 * ( v_next + v_previous)
                        distanceSpentInGap = numpy.linalg.norm( p_current - p_previous )
                        self.__extraTravelLength += distanceSpentInGap
                        self.__extraTravelDuration += int( numpy.around( distanceSpentInGap / averageSpeedInGap ) )
                        segmentStartingIndex = i
                        i += 1
                        p_current = p_next
                        v_current = v_next
                    else: # This is a jitter. Check whether we have a sequence of spikes and skip them.
                        j = i + 2
                        endOfSpikesFound = False
                        while j < len(tripData) -1 :
                            p_nnext = tripData[j]
                            v_nnext = numpy.linalg.norm( p_next - p_nnext )
                            p_nnnext = tripData[j+1]
                            v_nnnext = numpy.linalg.norm( p_nnnext - p_nnext )
                            if numpy.abs( v_nnnext - v_nnext ) < maxAcceleration :
                                averageSpeedInGap = 0.5 * ( v_nnext + v_previous )
                                distanceSpentInGap = numpy.linalg.norm( p_next - p_previous )
                                self.__extraTravelLength += distanceSpentInGap
                                self.__extraTravelDuration += int( numpy.around( distanceSpentInGap / averageSpeedInGap ) )
                                i = j
                                segmentStartingIndex = i - 1
                                p_current = p_next
                                v_current = averageSpeedInGap
                                endOfSpikesFound = True
                                break
                            j += 1
                            p_next = p_nnext

                        if endOfSpikesFound == False :
                            i = j
                            segmentStartingIndex = i
            
            p_previous = p_current
            v_previous = v_current
            i += 1
        
        if len(tripData) - segmentStartingIndex > 2 :
            segments.append( tripData[ segmentStartingIndex : len(tripData) ] )
        
        return segments

    
    def __removeAccuteAngleSegments( self, tripData ):
        maxAngle = 100 * numpy.pi / 180
        segments = []
        segmentStartingIndex = 0
        v_previous = tripData[1] - tripData[0]
        i = 2
        while i < len(tripData) :
            v_current = tripData[i] - tripData[i-1]
            angle = angleOfVectors( v_current, v_previous )
            if numpy.abs(angle) > maxAngle:
                if i - segmentStartingIndex > 2:
                    segments.append( tripData[ segmentStartingIndex: i-1 ] )
                self.__extraTravelDuration += 2
                self.__extraTravelLength += numpy.linalg.norm( tripData[i-2]-tripData[i] )
                segmentStartingIndex = i
                i += 2
                if i < len(tripData) + 1:
                    v_previous = tripData[i-1] - tripData[i-2]
                continue
            v_previous = v_current
            i += 1
            
        if len(tripData) - segmentStartingIndex > 2:
            segments.append( tripData[ segmentStartingIndex : len(tripData) ] )
    
        return segments
