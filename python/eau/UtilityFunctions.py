import numpy

def areaUnderLine( x, y ):
    result = 0
    for i in range( len(x)-1 ):
        ym = 0.5 * (y[i+1] + y[i] )
        dx = numpy.abs( x[i+1]-x[i] )
        result = result + dx*ym

    return result

# Gets the angle from y to x
def angleOfVectors( x, y ):
    mx = numpy.sqrt(x[0]**2 + x[1]**2)
    if mx == 0:
        return numpy.array( [ [ 1, 0 ],
                              [ 0, 1 ] ] )
    my = numpy.sqrt(y[0]**2 + y[1]**2)
    if my == 0:
        return numpy.array( [ [ 1, 0 ],
                              [ 0, 1 ] ] )
    magnitudeProduct = ( mx * my )
    sintheta = ( x[0]*y[1] - x[1]*y[0] ) / magnitudeProduct
    costheta = ( x[0]*y[0] + x[1]*y[1] ) / magnitudeProduct
    return numpy.array( [ [ costheta, -sintheta ],
                          [ sintheta, costheta ] ] )

# Rotates x by theta
def rotateVector( x, rotationMatrix ):
    return x.dot( rotationMatrix )
