import os
import os.path
import numpy
import pandas
import multiprocessing
from queue import Empty

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
    def loadSpeedStatisticsData( self, numberOfThreads = 7 ):
        
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
                speedStatistics = driver.speedStatistics()
                accelerationStatistics = driver.accelerationStatistics()
                angularStatistics = driver.angularStatistics()
                outputQueue.put(numpy.hstack((driver.id(), driver.numberOfTrips(), driver.zeroSegmentTrips(), speedStatistics, accelerationStatistics, angularStatistics)))
            return

        # Start the reading threads
        threads = []
        driversOutQueue = ctx.Queue()

        for i in range( numberOfThreads):
            thread = ctx.Process( target = readDataFunction, args = (driversInQueue, driversOutQueue, self.__dir ) )
            thread.start()
            threads.append( thread )

        drivers = numpy.zeros([numberOfDriversToProcess, 33])
        log = ProcessLogger( numberOfDriversToProcess )
        for i in range( numberOfDriversToProcess ):
            drivers[i] = driversOutQueue.get()
            log.taskEnded()

        for t in threads:
            t.terminate()

        columns=('id','ntrips','nosegs',
                 'sm05','ss05','sm25','ss25','sm50','ss50','sm75','ss75','sm95','ss95',
                 'am05','as05','am25','as25','am50','as50','am75','as75','am95','as95',
                 'tm05','ts05','tm25','ts25','tm50','ts50','tm75','ts75','tm95','ts95')
        return pandas.DataFrame(drivers, columns=columns)
        
    
