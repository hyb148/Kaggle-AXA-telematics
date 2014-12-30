import numpy
from .Segment import Segment

# Trip class
class Trip:
    
    # Constructor
    def __init__( self ):
        self.__rawData = []
        self.__segments = []
        return
    
    # Reading the input from a CSV file
    def readFromCSV( self, fileName ):
        self.__rawData = numpy.genfromtxt( fileName, dtype=float, delimiter=',', skip_header = 1 )
        # The velocity vectors
        self.__generateSegments()
        return

    # Returns the actual data
    def rawData( self ):
        return self.__rawData

    # The time duration of the trip
    def tripDuration( self ):
        return len( self.__rawData ) - 1

    # The trip segments
    def segments( self ):
        return self.__segments

    # The total distance traveled, derived from the segments
    def distanceTraveled( self ):
        result = 0
        for segment in self.__segments:
            result += segment.distanceTraveled()
        return result

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

    # The speed values
    def speedValues( self ):
        speedValues = numpy.array([])
        for segment in self.__segments:
            speedValues = numpy.hstack( (speedValues, segment.speedValues() ) )
        return speedValues

    # The speed values distribution
    def speedStatistics( self ):
        speedValues = self.speedValues()    
        if len( speedValues ) < 3:
            return (0,0,0,0,0)
        else:
            return numpy.percentile(speedValues,[5,25,50,75,95])

    # The acceleration values
    def accelerationValues( self ):
        accelerationValues = numpy.array([])
        for segment in self.__segments:
            accelerationValues = numpy.hstack( (accelerationValues, segment.accelerationValues() ) )
        return accelerationValues

    # The acceleration values distribution
    def accelerationStatistics( self ):
        accelerationValues = self.accelerationValues()
        if len( accelerationValues ) < 3:
            return (0,0,0,0,0)
        else:
            return numpy.percentile(accelerationValues,[5,25,50,75,95])

    # The direction angles
    def angularValues( self ):
        angularValues = numpy.array([])
        for segment in self.__segments:
            angularValues = numpy.hstack( (angularValues, segment.angularValues() ) )
        return angularValues
        

    # The direction angles distribution
    def angularStatistics( self ):
        angularValues = self.angularValues()
        if len( angularValues ) < 3:
            return (0,0,0,0,0)
        else:
            return numpy.percentile(angularValues,[5,25,50,75,95])
        

    # Fourier transformation of some quantity (velocity, angle)

    # Private function that generates the trip segments
    def __generateSegments(self):
        
        zeroSpeedTolerance = 1.5  # The speed below which it is considered that the speed is really zero
        zeroSpeedMaxCounts = 2 # After 2 seconds we mark a new segment
        maxSpeed = 60 # The maximum speed allowed in a segment
        maxAcceleration = 10 # The maximum acceleration allowed in a segment

        self.__segments = []
        i = 2
        segmentStartIndex = 0
        zeroSpeedCounter = 0
        v = numpy.diff( self.__rawData, axis=0)
        
        while i < len(self.__rawData):
            currentSpeed = v[i-1]
            currentAcceleration = currentSpeed - v[i-2]

            # Convert vectors to magnitudes
            currentSpeed = numpy.sqrt( currentSpeed[0]**2 + currentSpeed[1]**2)
            currentAcceleration = numpy.sqrt( currentAcceleration[0]**2 + currentAcceleration[1]**2 )

            # Check for zero speed
            if currentSpeed < zeroSpeedTolerance:
                zeroSpeedCounter += 1

            # Check if it comes right after a 0 speed segment
            # Check for abnormal speed
            # Check for abnormal acceleration
            if (zeroSpeedCounter >= zeroSpeedMaxCounts) or ( currentSpeed > maxSpeed ) or ( currentAcceleration > maxAcceleration ):
                
                # Mark the end of the segment, including up to the previous point,
                # as long as it has more than two points
                # Removing also the trailing zero-speed segments
                
                lastIndex = i - zeroSpeedCounter
                if lastIndex - segmentStartIndex > 2:
                    segment = Segment( self.__rawData[ segmentStartIndex : lastIndex ] )
                    self.__segments.append( segment )
                
                # Reinitiliase the conditions using the next point as the initial point.
                zeroSpeedCounter = 0
                segmentStartIndex = i
                i += 2
                continue
                
            i += 1 # Move to the next point in the trip

        # Treat the remaining trip as a segment
        i -= zeroSpeedCounter
        if i - segmentStartIndex > 2:
            segment = Segment( self.__rawData[ segmentStartIndex : i-1 ] )
            self.__segments.append( segment )
        
        return
        

    
