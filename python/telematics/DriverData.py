import os
import os.path
import queue
import threading

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
    def loadData( self, numberOfThreads ):
        self.__drivers = []
        
        # Put the driver directories in a queue
        driversInQueue = queue.Queue()
        driverdirs = os.listdir( self.__dir )
        for driverdir in driverdirs:
            driverId = int(driverdir)
            driver = Driver( driverId )
            driversInQueue.put( driver )

        # Define a read thread
        class readThread( threading.Thread ):
            def __init__(self, inputQueue, outputQueue, driversdir, log ):
                threading.Thread.__init__( self )
                self.__in = inputQueue
                self.__out = outputQueue
                self.__driversDir = driversdir
                self.__log = log
                return
            def run( self ):
                while True:
                    try:
                        driver = self.__in.get_nowait()
                        driver.readTripsFromDirectory( os.path.join( self.__driversDir, str(driver.id() ) ) )
                        self.__out.put( driver )
                        self.__log.taskEnded()
                    except queue.Empty:
                        break
                return

        # Start the reading threads
        threads = []
        driversOutQueue = queue.Queue()

        log = ProcessLogger( driversInQueue.qsize() )
        for i in range( numberOfThreads):
            thread = readThread( driversInQueue, driversOutQueue, self.__dir, log )
            thread.start()
            threads.append( thread )

        for t in threads: t.join()

        # Move the drivers data from the output queue to the list
        while not driversOutQueue.empty():
            self.__drivers.append( driversOutQueue.get() )

        return
        
        
