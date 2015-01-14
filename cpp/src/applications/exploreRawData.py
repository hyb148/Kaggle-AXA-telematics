# exploring the telematics data

import numpy
import os
import os.path

import matplotlib.pyplot as plt

# Gets the angle from y to x
def angleOfVectors( x, y ):
    mx = numpy.sqrt(x[0]**2 + x[1]**2)
    if mx == 0:
        return 0
    my = numpy.sqrt(y[0]**2 + y[1]**2)
    if my == 0:
        return 0
    mxy = mx * my
    sint = ( x[0]*y[1] - x[1]*y[0] ) / mxy
    cost = ( x[0]*y[0] + x[1]*y[1] ) / mxy
    if sint > 1: sint = 1
    if sint < -1: sint = -1
    if cost >= 0:
        return numpy.arcsin( sint )
    else:
        if sint > 0:
            return numpy.pi - numpy.arcsin( sint )
        else:
            if cost > 1: cost=1
            if cost < -1: cost=-1
            return -( numpy.arccos( cost ) )


# Function for plotting the raw data of the segments
def plotSegmentData( fig, segments ):

    # Clear the figure
    fig = plt.figure( fig.number )
    plt.clf()

    tripData = numpy.array([]).reshape([0,2])
    tAngles = numpy.array([])
    tSpeed = numpy.array([])
    tAcceleration = numpy.array([])
    
    for segment in segments:
        speedVectors = numpy.diff( segment, axis = 0 )
        speedValues = 3.6 * numpy.apply_along_axis( lambda x: numpy.sqrt( x[0]**2+x[1]**2), 1, speedVectors )
        if len(speedValues) > 1:
            accelerationValues = numpy.diff( speedValues ) / 3.6
            angles = numpy.zeros( len(speedVectors) - 1 )
            for i in range(len(angles)):
                v1 = speedVectors[i]
                v2 = speedVectors[i+1]
                angles[i] = angleOfVectors(v1,v2) * 180 / numpy.pi
            tAngles = numpy.hstack( (tAngles, angles) )
            tAcceleration = numpy.hstack( (tAcceleration, accelerationValues) )

        tSpeed = numpy.hstack( (tSpeed, speedValues) )
        tripData = numpy.vstack( (tripData, segment) )

    # Draw the raw data
    ax = fig.add_subplot(321)
    ax.plot( tripData[0:,0], tripData[0:,1], 'r.', alpha = 0.05 )
    ax.grid(True)
    ax = fig.add_subplot(322)
    ax.plot( numpy.arange(0,len(tripData)), tripData[0:,1], 'b.', alpha = 0.05 )
    ax.grid(True)
    ax = fig.add_subplot(323)
    ax.plot( tripData[0:,0], numpy.arange(0,len(tripData)), 'b.', alpha = 0.05 )
    ax.grid(True)
    ax = fig.add_subplot(324)
    ax.plot( numpy.arange(0,len(tAngles)), tAngles, 'r-' )
    ax.plot( numpy.arange(0,len(tAngles)), tAngles, 'r.' )
    ax.grid(True)
    ax = fig.add_subplot(325)
    ax.plot( numpy.arange(0,len(tSpeed)), tSpeed, 'g-' )
    ax.plot( numpy.arange(0,len(tSpeed)), tSpeed, 'g.', alpha=0.15 )
    ax.grid(True)
    ax = fig.add_subplot(326)
    ax.plot( numpy.arange(0,len(tAcceleration)), tAcceleration, 'b-' )
    ax.plot( numpy.arange(0,len(tAcceleration)), tAcceleration, 'b.', alpha=0.15 )
    ax.grid(True)

    return
    


class DataReader:
    def __init__( self, cppToPythonPipeName, pythonToCppPipeName ):
        self.__cppToPythonPipeName = cppToPythonPipeName
        self.__pythonToCppPipeName = pythonToCppPipeName
        return

    def exit( self ):
        fo = open( self.__pythonToCppPipeName, 'w' )
        fo.write( "exit" )
        fo.close()
        return

    def driverList( self ):
        result = []
        fo = open( self.__pythonToCppPipeName, 'w' )
        fo.write( "drivers" )
        fo.close()
        fi = open( self.__cppToPythonPipeName, 'r' )
        n = int( fi.readline() )
        for i in range(n):
            result.append( int(fi.readline()) )
        fi.close()
        return result
        
    def tripList( self, driverId ):
        result = []
        fo = open( self.__pythonToCppPipeName, 'w' )
        fo.write( "trips " + str(driverId) )
        fo.close()
        fi = open( self.__cppToPythonPipeName, 'r' )
        n = int( fi.readline() )
        for i in range(n):
            result.append( int(fi.readline()) )
        fi.close()
        return result

    def rawData( self, driverId, tripId ):
        fo = open( self.__pythonToCppPipeName, 'w' )
        fo.write( "rawdata " + str(driverId) + " " + str(tripId) )
        fo.close()
        result = numpy.array([]).reshape([0,2])
        fi = open( self.__cppToPythonPipeName, 'r' )
        n = int( fi.readline() )
        for i in range(n):
            data = fi.readline().strip().split()
            result = numpy.vstack((result, numpy.array([float(data[0]),float(data[1])]) ))
        fi.close()
        return result
        
    def segmentData( self, driverId, tripId ):
        fo = open( self.__pythonToCppPipeName, 'w' )
        fo.write( "segments " + str(driverId) + " " + str(tripId) )
        fo.close()
        sresult = []
        fi = open( self.__cppToPythonPipeName, 'r' )
        lines = fi.readlines()
        fi.close()
        nsegs = int(lines[0].strip())
        i = 1
        for s in range(nsegs):
            result = numpy.array([]).reshape([0,2])
            n = int(lines[i].strip())
            i += 1
            for ipoint in range(n):
                data = lines[i].strip().split()
                i += 1
                point = numpy.array([float(data[0]),float(data[1])])
                result = numpy.vstack((result, point ))
            sresult.append( result )
        return sresult
        


##########################################################################


dataReader = DataReader('cpptopythonpipe','pythontocpppipe')
drivers = dataReader.driverList()

#driverId = drivers[0]
driverId = 1

tripIds = dataReader.tripList( driverId )



# The Raw Data Window
fig1 = plt.figure(num=1, figsize=(7,10.5), facecolor='lightblue')
plt.get_current_fig_manager().set_window_title("Raw Data")

# The Results Window
fig2 = plt.figure(num=2, figsize=(7,10.5), facecolor='lightgray')
plt.get_current_fig_manager().set_window_title("Corrected Data")

# Looping over the trips
for tripId in tripIds:
#    if tripId not in (116,126,131,167,200,30,32,46,60,81): continue
    print( "Processing trip " + str(tripId) )

    # Get the raw data
    tripData = dataReader.rawData( driverId, tripId )

    # Plot the original raw data
    plotSegmentData( fig1, [tripData,] )

    atext = "Driver " + str(driverId) + "  -  Trip " + str(tripId)
    fig1.text(0.05, 0.97, atext, horizontalalignment='left')
    m,s = divmod(len(tripData)-1,60)
    h,m = divmod(m,60)
    atext = "Duration: " + str("%dh %02dm %02ds"%(h,m,s))
    fig1.text(0.05, 0.94, atext, horizontalalignment='left')
    plt.draw()

    # Get the corrected data
    segments = dataReader.segmentData( driverId, tripId )
    
    # Plot the data from the segments, after the corrections
    plotSegmentData( fig2, segments )
    atext = "Segments : " + str(len(segments))
    fig2.text(0.05, 0.97, atext, horizontalalignment='left')    
    plt.draw()
    
    # Show all the windows
    plt.show(block=False)

    r = input()
    if r != '':
        if r[0] == 'q':
            break
        
dataReader.exit()

plt.close(fig1.number)
plt.close(fig2.number)
