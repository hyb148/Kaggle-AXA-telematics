# exploring the telematics data

import telematics.DriverData as DriverData
import timeit
import numpy
import pickle

driverTopDir = 'drivers_compressed_data'
driverData = DriverData( driverTopDir )
t1 = timeit.default_timer()
drivers = driverData.loadAllData(7)
dt = int( numpy.around( timeit.default_timer() - t1, 0 ) )
print( "Data loaded in " + str(dt) + " seconds" )
#pickle.dump( drivers, open('driverStats.data','wb'))
input()
