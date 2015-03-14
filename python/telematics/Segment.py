import numpy
import scipy.fftpack
from .UtilityFunctions import angleOfVectors

# Segment class
class Segment:
    def __init__(self, coordinates ):
        self.__origin = coordinates[0]
        self.__v = numpy.diff( coordinates, axis=0)
        return
    
    def rawData( self ):
        result = []
        p = self.__origin
        result.append( p )
        for v in self.__v:
            p = p + v
            result.append(p)
        return numpy.array(result)            

    # The time duration of the segment
    def duration( self ):
        return len( self.__v )
    
    # The distance (and speed) vector
    def speedValues( self ):
        return numpy.apply_along_axis( lambda x: numpy.sqrt( x[0]**2+x[1]**2), 1, self.__v )

    # The distance traveled
    def distanceTraveled( self ):
        return numpy.sum( self.speedValues() )

    # The acceleration values
    def accelerationValues( self ):
        speedValues = self.speedValues()
        return numpy.diff( speedValues )

    # The angular values
    def angularValues( self ):
        angles = numpy.zeros( len(self.__v) - 1 )
        for i in range(len(angles)):
            angles[i] = angleOfVectors( self.__v[i], self.__v[i+1] )
        return angles
            
    # Fourier transformation of speed values
    def speedFFT( self ):
        sf = numpy.abs( scipy.fftpack.fft( self.speedValues() ) )
        n = int( numpy.around(0.1 + len(sf) / 2.0) )
        return sf[0:n]

    # Fourier transformation of anglular values
    def angleFFT( self ):
        af = numpy.abs( scipy.fftpack.fft( self.angularValues() ) )
        n = int( numpy.around(0.1 + len(af) / 2.0) )
        return af[0:n]

