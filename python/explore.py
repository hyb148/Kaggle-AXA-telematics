# exploring the telematics data

import os
import telematics
import eau
import numpy

#'''
pl = eau.Plotter()
trip = telematics.Trip()

v = numpy.array([])
a = numpy.array([])


for i in range(1,201):

    print("Processing driver 3, trip " + str(i) )
    
    trip.readFromCSV( 'drivers/103/' + str(i) + '.csv')

    vi = trip.speedSegments()
    v = numpy.concatenate( (v, vi[0:len(vi)-1]) )  # Can use vstack()
    a = numpy.concatenate( (a, trip.accelerationSegments()) )


condition = (v>2) & (v < 45) & ( numpy.abs(a) < 5 )

v,a = v[condition],a[condition]
pl.scatterPlotXY( v, a )
input("Press ENTER to continue")

pl.histogram( v )
input("Press ENTER to continue")
pl.histogram( a )
input("Press ENTER to continue")


del trip
del pl

'''


driversProcessed = 0

distances = []
directionVector = numpy.array([1,0])

driverdirs = os.listdir('drivers')
totalDrivers = len(driverdirs)

for driver in driverdirs:
    driversProcessed += 1
    print( "Processing driver " + str(driversProcessed) + "/" + str(totalDrivers) + " ("+ str( numpy.around( 100.0 * driversProcessed / totalDrivers, 2 ) ) + "%)")
    tripfiles = os.listdir( 'drivers/' + driver)
    tripsProcessed = 0
    for tripFile in tripfiles:
        fileName = 'drivers/' + driver + '/' + tripFile
        trip = telematics.Trip()
        trip.readFromCSV( fileName )
        rawData = trip.rawData()
        pivotPoint = rawData[len(rawData) / 2 ]
        theta = eau.angleOfVectors(directionVector, pivotPoint )
        endPoint = rawData[len(rawData) -1 ]
        transformedEndPoint = eau.rotateVector(endPoint,theta)
        if (len(distances) == 0 ):
            distances = [transformedEndPoint[1],]
        else:
            distances = numpy.vstack( ( distances, transformedEndPoint[1] ) )

        


    if driversProcessed == 1:
        break

plotter = eau.Plotter()
plotter.histogram(distances, bins=100)
        

'''
