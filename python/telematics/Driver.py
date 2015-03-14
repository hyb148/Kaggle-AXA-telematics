import os.path
import pickle
from .Trip import Trip

# Driver class
class Driver:
    def __init__( self, driverId ):
        self.__id = driverId
        self.__trips = {}
        self.__zeroSegmentTrips = 0
        return

    def __del__( self ):
        self.__clearTrips()
        return

    def id( self ):
        return self.__id

    def __clearTrips( self ):
        for i in self.__trips:
            o = self.__trips[i]
            self.__trips[i] = None
            del o
        self.__trips = {}
        self.__zeroSegmentTrips = 0

    def readTripsFromDirectory( self, directoryName ):
        self.__clearTrips()
        tripFile = os.path.join( directoryName, str(self.__id) + ".data" )
        file = open(tripFile, 'rb')
        while file.read(1):
            file.seek(-1,1)
            (tripId, tripData) = pickle.load( file )
            trip = Trip(tripId)
            trip.setData( tripData )
            if len(trip.segments()) == 0:
                self.__zeroSegmentTrips += 1
            self.__trips[tripId] = trip
        file.close()
        return

    # Returns the trip object given a trip id
    def getTrip( self, tripId ):
        return self.__trips[ tripId ]

    # Returns the number of zero segment trips
    def zeroSegmentTrips( self ):
        return self.__zeroSegmentTrips

    # Returns the total number of trips
    def numberOfTrips( self ):
        return len( self.__trips )
