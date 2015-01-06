# exploring the telematics data

import numpy
import os
import os.path
import pickle
import matplotlib.pyplot as plt

import eau

driverTopDir = 'drivers_compressed_data'
driverId = 1

# Read the trip files for the specified driver
driverFileName = os.path.join( driverTopDir, str(driverId)+".data")
file = open(driverFileName, 'rb')
trips=[]
while file.read(1):
    file.seek(-1,1)
    trips.append( pickle.load(file) )
file.close()

fig = plt.figure(num=1, figsize=(8,10), dpi=80, facecolor='lightblue')
# Looping over the trips
for trip in trips:
    tripId = trip[0]
    print( "Processing trip " + str(tripId) )
    tripData = trip[1]
    speedVectors = numpy.diff( tripData, axis = 0 )
    speedValues = 3.6 * numpy.apply_along_axis( lambda x: numpy.sqrt( x[0]**2+x[1]**2), 1, speedVectors )
    angles = numpy.zeros( len(speedVectors) - 1 )
    for i in range(len(angles)):
        v1 = speedVectors[i]
        v2 = speedVectors[i+1]
        angles[i] = eau.angleOfVectors(v1,v2) * 180 / numpy.pi
    plt.clf()
    plt.subplot(321)
    plt.plot( tripData[0:,0], tripData[0:,1], 'ro', alpha = 0.05 )
    plt.subplot(322)
    plt.plot( numpy.arange(0,len(tripData)), tripData[0:,1], 'bo', alpha = 0.05 )
    plt.subplot(323)
    plt.plot( tripData[0:,0], numpy.arange(0,len(tripData)), 'bo', alpha = 0.05 )
    plt.subplot(324)
    plt.plot( numpy.arange(0,len(angles)), angles, 'r-' )
    plt.subplot(313)
    plt.plot( numpy.arange(0,len(speedValues)), speedValues, 'g-' )

    atext = "Driver " + str(driverId) + "  -  Trip " + str(tripId)
    plt.text(0.05, 0.98, atext, transform=fig.transFigure, horizontalalignment='left')

    m,s = divmod(len(speedVectors),60)
    h,m = divmod(m,60)
    atext = "Duration: " + str("%dh %02dm %02ds"%(h,m,s))
    plt.text(0.05, 0.95, atext, transform=fig.transFigure, horizontalalignment='left')

    plt.draw()
    plt.show(block=False)
    input()

plt.close(1)
