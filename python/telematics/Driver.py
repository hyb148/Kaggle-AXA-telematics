import threading
import os
import os.path
from .Trip import Trip

# Driver class
class Driver:
    def __init__( self, driverId ):
        self.__id = driverId
        self.__trips = {}
        self.__lock = threading.Lock()
        return

    def __del__( self ):
        self.__clearTrips()
        del self.__lock
        return

    def id( self ):
        return self.__id

    def __clearTrips( self ):
        for i in self.__trips:
            o = self.__trips[i]
            self.__trips[i] = None
            del o
        self.__trips = {}

    def readTripsFromDirectory( self, directoryName ):
        self.__lock.acquire()        
        self.__clearTrips()
        tripFiles = os.listdir( directoryName )
        for tripFile in tripFiles:
            tripId = int(tripFile.split('.')[0])
            fileName = os.path.join( directoryName, tripFile )
            trip = Trip( tripId )
            trip.readFromCSV( fileName )
            self.__trips[tripId] = trip
        self.__lock.release()
        return

    def numberOfTrips( self ):
        return len(self.__trips)

    def getTrip( self, tripId ):
        return self.__trips[ tripId ]
