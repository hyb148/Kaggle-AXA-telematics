# exploring the telematics data

import numpy
import os
import os.path

import matplotlib.pyplot as plt

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

    def accelerationValues( self, driverId, tripId ):
        result = []
        fo = open( self.__pythonToCppPipeName, 'w' )
        fo.write( "acceleration " + str(driverId) + " " + str(tripId) )
        fo.close()
        fi = open( self.__cppToPythonPipeName, 'r' )
        n = int( fi.readline() )
        for i in range(n):
            result.append( float(fi.readline()) )
        fi.close()
        return result
        
    def travelDuration( self, driverId, tripId ):
        fo = open( self.__pythonToCppPipeName, 'w' )
        fo.write( "travelDuration " + str(driverId) + " " + str(tripId) )
        fo.close()
        fi = open( self.__cppToPythonPipeName, 'r' )
        v = float( fi.readline() )
        fi.close()
        return v
        
    def travelLength( self, driverId, tripId ):
        fo = open( self.__pythonToCppPipeName, 'w' )
        fo.write( "travelLength " + str(driverId) + " " + str(tripId) )
        fo.close()
        fi = open( self.__cppToPythonPipeName, 'r' )
        v = float( fi.readline() )
        fi.close()
        return v
        
    def distanceOfEndPoint( self, driverId, tripId ):
        fo = open( self.__pythonToCppPipeName, 'w' )
        fo.write( "distanceOfEndPoint " + str(driverId) + " " + str(tripId) )
        fo.close()
        fi = open( self.__cppToPythonPipeName, 'r' )
        v = float( fi.readline() )
        fi.close()
        return v
        
    def speedValues( self, driverId, tripId ):
        result = numpy.array([]).reshape([0,1])
        fo = open( self.__pythonToCppPipeName, 'w' )
        fo.write( "speed " + str(driverId) + " " + str(tripId) )
        fo.close()
        fi = open( self.__cppToPythonPipeName, 'r' )
        n = int( fi.readline() )
        for i in range(n):
            result = numpy.vstack( ( result, float(fi.readline() ) ) )
        fi.close()
        return result

    def accelerationValues( self, driverId, tripId ):
        result = numpy.array([]).reshape([0,1])
        fo = open( self.__pythonToCppPipeName, 'w' )
        fo.write( "acceleration " + str(driverId) + " " + str(tripId) )
        fo.close()
        fi = open( self.__cppToPythonPipeName, 'r' )
        n = int( fi.readline() )
        for i in range(n):
            result = numpy.vstack( ( result, float(fi.readline() ) ) )
        fi.close()
        return result

    def directionValues( self, driverId, tripId ):
        result = numpy.array([]).reshape([0,1])
        fo = open( self.__pythonToCppPipeName, 'w' )
        fo.write( "direction " + str(driverId) + " " + str(tripId) )
        fo.close()
        fi = open( self.__cppToPythonPipeName, 'r' )
        n = int( fi.readline() )
        for i in range(n):
            result = numpy.vstack( ( result, float(fi.readline() ) ) )
        fi.close()
        return result

    def speedAccelerationDirectionValues( self, driverId, tripId ):
        result = numpy.array([]).reshape([0,3])
        fo = open( self.__pythonToCppPipeName, 'w' )
        fo.write( "speedAccelerationDirection " + str(driverId) + " " + str(tripId) )
        fo.close()
        fi = open( self.__cppToPythonPipeName, 'r' )
        n = int( fi.readline() )
        for i in range(n):
            line = fi.readline().strip().split()
            line = numpy.array([float(line[0]),float(line[1]),float(line[2])])
            result = numpy.vstack( ( result, line ) )
        fi.close()
        return result

        


##########################################################################


dataReader = DataReader('cpptopythonpipe','pythontocpppipe')
drivers = dataReader.driverList()

#driverId = drivers[0]
driverId = 2

tripIds = dataReader.tripList( driverId )


# The Raw Data Window
fig = plt.figure(num=1, figsize=(20,10.5), facecolor='lightblue')
plt.get_current_fig_manager().set_window_title("Trip summary")

# Looping over the trips
for tripId in tripIds:
    print( "Processing trip " + str(tripId) )

    travelDuration = dataReader.travelDuration( driverId, tripId )
    travelLength = dataReader.travelLength( driverId, tripId )
    distanceOfEndPoint = dataReader.distanceOfEndPoint( driverId, tripId )
    travelToDistance = travelLength / distanceOfEndPoint
    
    values = dataReader.speedAccelerationDirectionValues( driverId, tripId )

    if len(values) > 1 :
        ax = fig.add_subplot(331)
        ax.plot( numpy.arange(0,len(values) ), values[0:,0], 'g-' )
        ax.set_ylabel('Speed $m/s$')
        ax.grid(True)
    
        ax = fig.add_subplot(332)
        ax.plot( numpy.arange(0,len(values) ), values[0:,1], 'b-' )
        ax.set_ylabel('Acceleration $m/s^2$')
        ax.grid(True)

        ax = fig.add_subplot(333)
        ax.plot( numpy.arange(0,len(values) ), values[0:,2], 'r-' )
        ax.set_ylabel('Direction $rad$')
        ax.grid(True)

        ax = fig.add_subplot(334)
        ax.plot( values[0:,0], values[0:,1], 'g.', alpha=0.3 )
        ax.set_xlabel('Speed $m/s$')
        ax.set_ylabel('Acceleration $m/s^2$')
        ax.grid(True)

        ax = fig.add_subplot(335)
        ax.plot( values[0:,0], values[0:,2], 'b.', alpha=0.3 )
        ax.set_xlabel('Speed $m/s$')
        ax.set_ylabel('Direction $rad$')
        ax.grid(True)

        ax = fig.add_subplot(336)
        ax.plot( values[0:,1], values[0:,2], 'r.', alpha=0.3 )
        ax.set_xlabel('Acceleration $m/s^2$')
        ax.set_ylabel('Direction $rad$')
        ax.grid(True)

        ax = fig.add_subplot(337)
        ax.hist( values[0:,0], 30, normed=True, facecolor='green' )
        ax.set_xlabel('Speed $m/s$')
        ax.grid(True)

        ax = fig.add_subplot(338)
        ax.hist( values[0:,1], 30, normed=True, facecolor='blue' )
        ax.set_xlabel('Acceleration $m/s^2$')
        ax.grid(True)

        ax = fig.add_subplot(339)
        ax.hist( values[0:,2], 30, normed=True, facecolor='red' )
        ax.set_xlabel('Direction $rad$')
        ax.grid(True)

    m,s = divmod( travelDuration, 60)
    h,m = divmod( m, 60 )
    text = "Driver : " + str(driverId)
    fig.text(0.45,0.97, text, horizontalalignment='left')
    text = "Trip : " + str(tripId)
    fig.text(0.45,0.94, text, horizontalalignment='left')
    text = "Travel duration : " + str("%dh %02dm %02ds"%(h,m,s))
    fig.text(0.05,0.97, text, horizontalalignment='left')
    text = "Travel length   : " + str(numpy.around(travelLength / 1000.0 , 1 ) ) + " km"
    fig.text(0.05,0.94, text, horizontalalignment='left')
    text = "Travel to distance : " + str(numpy.around(travelToDistance , 2 ) )
    fig.text(0.20,0.94, text, horizontalalignment='left')
    plt.draw()
    plt.show(block=False)

    r = input()
    plt.clf()
    if r != '':
        if r[0] == 'q':
            break
        
dataReader.exit()

plt.close(fig.number)
