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
        self.__v = numpy.diff( self.__rawData, axis=0)
        self.__generateSegments()
        return

    # Size of the trip data
    def size( self ):
        return len( self.__rawData )

    # Returns the actual data
    def rawData( self ):
        return self.__rawData

    # The time duration of the trip
    def duration( self ):
        return len( self.__rawData ) - 1

    # The destination distance
    def destinationDistance( self ):
        (x,y) = self.__rawData[ len(self.__rawData) - 1]
        return numpy.sqrt( x**2 + y**2)

    # The average speed along a straight line
    def speedAlongStraightLine( self ):
        return self.destinationDistance() / self.duration()

    # The distance (and speed) vector
    def speedSegments( self ):
        return numpy.apply_along_axis( lambda x: numpy.sqrt( x[0]**2+x[1]**2), 1, self.__v )

    # The distance traveled
    def travelDistance( self ):
        return numpy.sum( self.speedSegments() )

    # The mean traveling speed
    def meanSpeed( self ):
        return self.travelDistance() / self.size()

    # The travel to distance
    def travelToDistance( self ):
        return self.travelDistance() / self.destinationDistance()

    # Private function that generates the trip segments
    def __generateSegments(self):
        
        zeroSpeedTolerance = 1  # The speed below which it is considered that the speed is really zero
        zeroSpeedMaxCounts = 10 # After 10 seconds we mark a new segment
        maxSpeed = 60 # The maximum speed allowed in a segment
        maxAcceleration = 10 # The maximum acceleration allowed in a segment

        self.__segments = []
        i = 2
        segmentStartIndex = 0
        zeroSpeedCounter = 0
        
        while i < len(self.__rawData):
            currentSpeed = self.__v[i-1]
            currentAcceleration = currentSpeed - self.__v[i-2]

            # Convert vectors to magnitudes
            currentSpeed = numpy.sqrt( currentSpeed[0]**2 + currentSpeed[1]**2)
            currentAcceleration = numpy.sqrt( currentAcceleration[0]**2 + currentAcceleration[1]**2 )

            # Check for zero speed
            if currentSpeed < zeroSpeedTolerance:
                zeroSpeedCounter += 1

            # Mark the end of the segment
            if zeroSpeedCounter >= zeroSpeedMaxCounts:
                lastIndex = i - zeroSpeedCounter
                if lastIndex - segmentStartIndex > 2:
                    segment = Segment( self.__rawData[ segmentStartIndex : lastIndex ] )
                    self.__segments.append( segment )
                    
                # Reinitiliase the conditions using the next point as the initial point.
                segmentStartIndex = i
                i += 2
                continue
                

            # Check for abnormaly high speed or accelaration
            if ( currentSpeed > maxSpeed ) or ( currentAcceleration > maxAcceleration ):
                # Mark the end of the segment, including up to the previous point, as long as it has more than two points
                if i - segmentStartIndex > 2:
                    segment = Segment( self.__rawData[ segmentStartIndex : i-1 ] )
                    self.__segments.append( segment )
                
                # Reinitiliase the conditions using the next point as the initial point.
                segmentStartIndex = i
                i += 2
                continue
                
            i += 1 # Move to the next point in the trip

        # Treat the remaining trip as a segment
        if i - segmentStartIndex > 2:
            segment = Segment( self.__rawData[ segmentStartIndex : i-1 ] )
            self.__segments.append( segment )
        
        return
        

    
