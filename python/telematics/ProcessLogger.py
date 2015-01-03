# Simple process logger class

import threading
import sys
import numpy

class ProcessLogger:
    def __init__( self, numberOfTasks, outputStream = sys.stdout ):
        self.__numberOfTasks = numberOfTasks
        self.__tasksCompleted = 0
        self.__lock = threading.Lock()
        self.__outputStream = outputStream
        return

    def __del__( self ):
        del self.__lock

    def taskEnded( self ):
        self.__lock.acquire()
        if self.__tasksCompleted == self.__numberOfTasks:
            self.__lock.release()
            return
        self.__tasksCompleted += 1
        percentage = numpy.around( 100.0 * self.__tasksCompleted / self.__numberOfTasks, 1 )
        if self.__tasksCompleted > 1:
            self.__outputStream.write( '\r' )
        self.__outputStream.write( "Tasks processed: " + str(self.__tasksCompleted) + " / " + str( self.__numberOfTasks) + " ( " + str(percentage) + "% )" )
        if self.__tasksCompleted == self.__numberOfTasks:
            self.__outputStream.write( '\n' )
        self.__outputStream.flush()
        self.__lock.release()
        return
