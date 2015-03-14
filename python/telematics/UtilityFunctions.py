import numpy

def areaUnderLine( x, y ):
    result = 0
    for i in range( len(x)-1 ):
        ym = 0.5 * (y[i+1] + y[i] )
        dx = numpy.abs( x[i+1]-x[i] )
        result = result + dx*ym

    return result

# Gets the angle from y to x as rotation matrix
def angleOfVectorsAsRotationMatrix( x, y ):
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

# Gets the angle from y to x
def angleOfVectors( x, y ):
    mx = numpy.linalg.norm(x)
    if mx == 0:
        return 0
    my = numpy.linalg.norm(y)
    if my == 0:
        return 0
    mxy = mx * my
    sint = ( x[0]*y[1] - x[1]*y[0] ) / mxy
    if sint > 1: sint = 1
    if sint < -1: sint = -1
    cost = ( x[0]*y[0] + x[1]*y[1] ) / mxy
    if cost > 1: cost = 1
    if cost < -1: cost = -1
    if cost >= 0:
        return numpy.arcsin( sint )
    else:
        if sint > 0:
            return numpy.pi - numpy.arcsin( sint )
        else:
            if cost > 1: cost=1
            if cost < -1: cost=-1
            return -( numpy.arccos( cost ) )    


# Rotates x by a rotation matrix
def rotateVectorByMatrix( x, rotationMatrix ):
    return x.dot( rotationMatrix )


def rotateVector( x, theta ):
    cost = numpy.cos(theta)
    sint = numpy.sin(theta)
    R = numpy.array( [ [cost, -sint],
                       [sint, cost] ] )
    return rotateVectorByMatrix( x, R )
