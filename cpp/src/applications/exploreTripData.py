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
driverId = 1

tripIds = dataReader.tripList( driverId )


# The Raw Data Window
fig1 = plt.figure(num=1, figsize=(7,10.5), facecolor='lightblue')
plt.get_current_fig_manager().set_window_title("Raw Data")

# Looping over the trips
for tripId in tripIds:
#    if tripId not in (116,126,131,167,200,30,32,46,60,81): continue
    print( "Processing trip " + str(tripId) )

    travelDuration = dataReader.travelDuration( driverId, tripId )
    values = dataReader.speedAccelerationDirectionValues( driverId, tripId )

    #if travelDuration == 0:
    print( "Travel duration : " + str(travelDuration) )
    print( values )

    r = input()
    if r != '':
        if r[0] == 'q':
            break
        
dataReader.exit()

plt.close(fig1.number)
