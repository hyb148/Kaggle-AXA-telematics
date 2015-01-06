# exploring the telematics data

import telematics.Driver as Driver
import numpy
import matplotlib.pyplot as plt

fig = plt.figure(1)

driver = Driver(1)
driver.readTripsFromDirectory('drivers/1')
for i in range( driver.numberOfTrips() ):
    print("Processing trip " + str(i+1) )
    trip = driver.getTrip(i+1)
    segments = trip.segments()
    for segment in segments:
        input("next segment")
        speedFFT = segment.speedFFT()
        angleFFT = segment.angleFFT()
        plt.clf()
        plt.subplot(211)
        plt.bar(list(range(len(speedFFT))),speedFFT)
        plt.subplot(212)
        plt.bar(list(range(len(angleFFT))),angleFFT)
        plt.draw()
        plt.show(block=False)

input("done")
