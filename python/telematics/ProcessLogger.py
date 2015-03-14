# Simple process logger class
import sys
import numpy

class ProcessLogger:
    def __init__( self, numberOfTasks, message = "Tasks processed: ", outputStream = sys.stdout ):
        self.__numberOfTasks = numberOfTasks
        self.__tasksCompleted = 0
        self.__outputStream = outputStream
        self.__message = message
        return
    
    def taskEnded( self ):
        if self.__tasksCompleted == self.__numberOfTasks:
            return
        self.__tasksCompleted += 1
        percentage = numpy.around( 100.0 * self.__tasksCompleted / self.__numberOfTasks, 1 )
        if self.__tasksCompleted > 1:
            self.__outputStream.write( '\r' )
        self.__outputStream.write( self.__message + str(self.__tasksCompleted) + " / " + str( self.__numberOfTasks) + " ( " + str(percentage) + "% )" )
        if self.__tasksCompleted == self.__numberOfTasks:
            self.__outputStream.write( '\n' )
        self.__outputStream.flush()
        return
