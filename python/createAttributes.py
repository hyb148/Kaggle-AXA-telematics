# exploring the telematics data

from telematics import *
import pickle

driverTopDir = 'drivers_compressed_data'
outputFileName = 'tripAttributes.data'

driverData = DriverData(driverTopDir)
tripData = driverData.loadAttributes()
outputFile = open(outputFileName,'wb')
pickle.dump(tripData, outputFile)
outputFile.close()
del outputFile
