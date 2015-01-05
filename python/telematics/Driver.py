import os
import os.path
import numpy
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
            self.__trips[tripId] = trip
        file.close()
        return

    def numberOfTrips( self ):
        return len(self.__trips)

    def getTrip( self, tripId ):
        return self.__trips[ tripId ]

    # Returns the number of zero segment trips
    def zeroSegmentTrips( self ):
        return self.__zeroSegmentTrips

    # Returns the total number of trips
    def numberOfTrips( self ):
        return len( self.__trips )

    # Collects the speed statistics
    def speedStatistics( self ):
        quantiles = numpy.zeros([(self.numberOfTrips() - self.zeroSegmentTrips() ), 5] )
        j = 0
        for i in self.__trips:
            trip = self.__trips[i]
            if trip.numberOfSegments() == 0:
                continue
            else:
                quantiles[j] = trip.speedStatistics()
                j += 1        
        return numpy.array([( numpy.mean( quantiles[:,k]), numpy.std( quantiles[:,k]) ) for k in range(5)]).reshape(10)

    # Collects the acceleration statistics
    def accelerationStatistics( self ):
        quantiles = numpy.zeros([(self.numberOfTrips() - self.zeroSegmentTrips() ), 5] )
        j = 0
        for i in self.__trips:
            trip = self.__trips[i]
            if trip.numberOfSegments() == 0:
                continue
            else:
                quantiles[j] = trip.accelerationStatistics()
                j += 1        
        return numpy.array([( numpy.mean( quantiles[:,k]), numpy.std( quantiles[:,k]) ) for k in range(5)]).reshape(10)

    # Collects the acceleration statistics
    def angularStatistics( self ):
        quantiles = numpy.zeros([(self.numberOfTrips() - self.zeroSegmentTrips() ), 5] )
        j = 0
        for i in self.__trips:
            trip = self.__trips[i]
            if trip.numberOfSegments() == 0:
                continue
            else:
                quantiles[j] = trip.angularStatistics()
                j += 1        
        return numpy.array([( numpy.mean( quantiles[:,k]), numpy.std( quantiles[:,k]) ) for k in range(5)]).reshape(10)

    
