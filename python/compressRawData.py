# compresses the driver data

import pickle
import os
import os.path
import numpy
from telematics import ProcessLogger


driversInputDir = "drivers"
driversOutputDir = "drivers_compressed_data"

driverDirs = os.listdir(driversInputDir)
log = ProcessLogger( len(driverDirs) )
for driverDir in driverDirs:
    driverId = int(driverDir)
    outputFile = open( os.path.join( driversOutputDir, driverDir + ".data" ), 'wb' )
    tripFiles = os.listdir( os.path.join( driversInputDir, driverDir ) )
    for tripFile in tripFiles:
        fileName = os.path.join(driversInputDir, driverDir, tripFile )
        tripId = int(tripFile.split('.')[0])
        tripData = numpy.genfromtxt( fileName, dtype=float, delimiter=',', skip_header = 1 )
        tripStruct=(tripId, tripData)
        pickle.dump( tripStruct, outputFile )
    outputFile.close()
    del outputFile
    log.taskEnded()

        
