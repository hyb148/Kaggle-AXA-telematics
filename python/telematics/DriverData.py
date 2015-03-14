import os
import numpy
import pandas
import multiprocessing

from .Driver import Driver
from .ProcessLogger import ProcessLogger

# Main class holding all the driver data
class DriverData:
    def __init__( self, driversDirectory ):
        self.__dir = driversDirectory
        return

    # Loads all the data from the files for all drivers
    def loadAllData( self, numberOfThreads = 7 ):
        
        # Put the driver directories in a queue
        ctx = multiprocessing.get_context('fork')
        driversInQueue = ctx.Queue()
        driverFiles = os.listdir( self.__dir )
        numberOfDriversToProcess = 0
        for driverFile in driverFiles:
            driverId = int(int(driverFile.split('.')[0]))
            driver = Driver( driverId )
            driversInQueue.put( driver )
            numberOfDriversToProcess += 1

        # The thread reading function
        def readDataFunction( inputQueue, outputQueue, driverTopDir ):
            while True:
                driver = inputQueue.get()
                driver.readTripsFromDirectory( self.__dir )
                outputQueue.put( driver )
            return

        # Start the reading threads
        threads = []
        driversOutQueue = ctx.Queue()

        for i in range( numberOfThreads):
            thread = ctx.Process( target = readDataFunction, args = (driversInQueue, driversOutQueue, self.__dir ) )
            thread.start()
            threads.append( thread )

        drivers = []
        log = ProcessLogger( numberOfDriversToProcess )
        for i in range( numberOfDriversToProcess ):
            drivers.append( driversOutQueue.get() )
            log.taskEnded()

        for t in threads:
            t.terminate()
            
        return drivers


    # Loads the speed statistics data per driver
    def loadAttributes( self, numberOfThreads = 7 ):
        
        # Put the driver directories in a queue
        ctx = multiprocessing.get_context('fork')
        driversInQueue = ctx.Queue()
        driverFiles = os.listdir( self.__dir )
        numberOfDriversToProcess = 0
        for driverFile in driverFiles:
            driverId = int(int(driverFile.split('.')[0]))
            driver = Driver( driverId )
            driversInQueue.put( driver )
            numberOfDriversToProcess += 1
        
        # The thread reading function
        def readDataFunction( inputQueue, outputQueue, driverTopDir ):
            while True:
                driver = inputQueue.get()
                driver.readTripsFromDirectory( self.__dir )
                numberOfTrips = driver.numberOfTrips()
                tripData = []
                for i in range(numberOfTrips):
                    trip = driver.getTrip( i + 1 )
                    values, labels = trip.attributes()
                    tripData.append( ( trip.id(), values, labels ) )
                outputQueue.put( (driver.id(), tripData ) )
            return

        # Start the reading threads
        threads = []
        driversOutQueue = ctx.Queue()

        for i in range( numberOfThreads):
            thread = ctx.Process( target = readDataFunction, args = (driversInQueue, driversOutQueue, self.__dir ) )
            thread.start()
            threads.append( thread )

        # Set up the logger
        log = ProcessLogger( numberOfDriversToProcess, "Drivers processed : " )
        outputData = []
        labels = []
        for i in range( numberOfDriversToProcess ):
            driverId, tripData = driversOutQueue.get()
            # Loop over the trips for this driver
            for trip in tripData:
                if len(labels) == 0: # This is the first entry. Retrieve the header.
                    labels.append( 'driverId' )
                    labels.append( 'tripId' )
                    for label in trip[2]:
                        labels.append(label)
                    outputData = numpy.array([]).reshape(0,len(labels))
                tripId = trip[0]
                attributes = trip[1]
                tripRow = numpy.hstack( (driverId, tripId, attributes) )
                outputData = numpy.vstack( (outputData, tripRow) )
            log.taskEnded()

        for t in threads:
            t.terminate()

        return pandas.DataFrame(outputData, columns=labels)
