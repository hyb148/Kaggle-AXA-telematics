import numpy
import scipy.fftpack

# Segment class
class Segment:
    def __init__(self, coordinates ):
        self.__v = numpy.diff( coordinates, axis=0)
        return

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
            v1 = self.__v[i]
            v2 = self.__v[i+1]
            mv1 = numpy.sqrt( v1[0]**2 + v1[1]**2)
            if mv1 == 0:
                continue
            mv2 = numpy.sqrt( v2[0]**2 + v2[1]**2)
            if mv2 == 0:
                continue
            mv = mv1 * mv2
            sint = ( v1[0]*v2[1] - v1[1]*v2[0] ) / mv
            cost = ( v1[0]*v2[0] + v1[0]*v2[0] ) / mv
            if sint > 1:
                sint = 1
            if sint < -1:
                sint = -1
            theta = numpy.arcsin( sint )
            if cost >= 0:
                angles[i] = theta
            else:
                angles[i] = numpy.pi - theta
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
