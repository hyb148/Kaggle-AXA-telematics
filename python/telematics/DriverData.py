import os
import os.path
import multiprocessing
from queue import Empty

from .Driver import Driver
from .ProcessLogger import ProcessLogger

# Main class holding all the driver data
class DriverData:
    def __init__( self, driversDirectory ):
        self.__dir = driversDirectory
        self.__drivers = []
        return

    # Returns the list of the drivers
    def drivers( self ):
        return self.__drivers

    # Loads the data from the files for all drivers
    def loadData( self, numberOfThreads = 3 ):
        self.__drivers = []

        ctx = multiprocessing.get_context('fork')
        
        # Put the driver directories in a queue
        driversInQueue = ctx.Queue()
        driverdirs = os.listdir( self.__dir )
        for driverdir in driverdirs:
            driverId = int(driverdir)
            driver = Driver( driverId )
            driversInQueue.put( driver )

        # The thread reading function
        def readDataFunction( inputQueue, outputQueue, driverTopDir ):
            while True:
                try:
                    driver = inputQueue.get_nowait()
                    driver.readTripsFromDirectory( os.path.join( driverTopDir, str(driver.id() ) ) )
                    outputQueue.put( driver )
                except:
                    break
            return

        # Start the reading threads
        threads = []
        driversOutQueue = ctx.Queue()

        for i in range( numberOfThreads):
            thread = ctx.Process( target = readDataFunction, args = (driversInQueue, driversOutQueue, self.__dir ) )
            thread.start()
            threads.append( thread )

        log = ProcessLogger( len(driverdirs) )
        for i in range( len(driverdirs) ):
            self.__drivers.append( driversOutQueue.get() )
            log.taskEnded()

        for t in threads: t.join()

        return
        
        
