# exploring the telematics data

import numpy
import os
import os.path
import pickle

import matplotlib.pyplot as plt

import eau

driverTopDir = 'drivers_compressed_data'
driverId = 1
#driverId = 12
#driverId = 123
#driverId = 1234

# Function filling in the gaps in a trip, correcting the jitter points
def findMissingData( tripData ):
    
    maxAcceleration = 5 # The maximum acceleration allowed in a segment
    speedToTrigger = 10 # A jump of 50 metres
    jitterTolerance = 1 # the number of seconds on the jitter

    missingData=[]
    jitterPoints = []

    v_previous = 0
    p_previous = tripData[0]
    i = 1
    while i < len(tripData) - 1:
        p_current = tripData[i]
        v_current = p_current - p_previous
        v_current = numpy.sqrt( v_current[0]**2 + v_current[1]**2 )

        # Check for abrupt high speed and high acceleration followed by normal speed
        if ( numpy.abs( v_current - v_previous ) > maxAcceleration ) and v_current > speedToTrigger:
            p_next = tripData[i + 1]
            v_next = p_next - p_current
            v_next = numpy.sqrt( v_next[0]**2 + v_next[1]**2 )
            if ( numpy.abs( v_current - v_next ) > maxAcceleration ):  # We may need to relax this in order to allow for smaller decceleration
                averageSpeed = 0.5 * (v_previous + v_next)
                secondsInGap = int( numpy.floor( v_current / averageSpeed ) )
                if secondsInGap > jitterTolerance:  # This is a gap
                    numberOfMiniSegments = secondsInGap + 1
                    speedIncrement = (v_next - v_previous)/ numberOfMiniSegments
                    directionVector = p_next - p_previous
                    directionVector /= numpy.sqrt( directionVector[0]**2 + directionVector[1]**2 )
                    dataPoints = numpy.array([]).reshape([0,2])
                    pp_point = p_previous
                    for s in range(secondsInGap):
                        speedInGap = v_previous + (s+1) * speedIncrement
                        newPoint = pp_point + speedInGap * directionVector
                        pp_point = newPoint
                        dataPoints = numpy.vstack( (dataPoints, newPoint) )
                    missingData.append([i, dataPoints])

                    v_previous = v_next - speedIncrement
                    p_previous = dataPoints[len(dataPoints)-1]
                    i += 1
                    continue
                else:  # This is a jitter. Need to correct  the values
                    jitterPoints.append( [ i, 0.5*(p_next+p_previous) ] ) # We can do better taking into account the previous speeds
                    
                    v_previous = 0.5*(p_next - p_previous)
                    v_previous = numpy.sqrt( v_previous[0]**2 + v_previous[1]**2 )
                    p_previous = p_next
                    i += 2
                    continue
                    
        
        p_previous = p_current
        v_previous = v_current
        i += 1

    _tripData = tripData
    correctedTripData = numpy.array([]).reshape([0,2])
    for jitterPoint in jitterPoints:
        _tripData[ jitterPoint[0] ] = jitterPoint[1]
    i = 0
    for gap in missingData:
        while i < gap[0]:
            correctedTripData = numpy.vstack( ( correctedTripData, _tripData[i] ) )
            i += 1
        correctedTripData = numpy.vstack( ( correctedTripData, gap[1] ) )
    while i < len(tripData ):
        correctedTripData = numpy.vstack( ( correctedTripData, _tripData[i] ) )
        i += 1

    return ( correctedTripData, len(missingData), len(jitterPoints) )


# Function to apply the removal of the zero speed segments
def removeZeroSpeedSegments( tripData ):

    return (tripData, 0)


# Function for plotting the data
def plotData( fig, tripData ):
    speedVectors = numpy.diff( tripData, axis = 0 )
    speedValues = 3.6 * numpy.apply_along_axis( lambda x: numpy.sqrt( x[0]**2+x[1]**2), 1, speedVectors )
    accelerationValues = numpy.diff( speedValues ) / 3.6
    angles = numpy.zeros( len(speedVectors) - 1 )
    for i in range(len(angles)):
        v1 = speedVectors[i]
        v2 = speedVectors[i+1]
        angles[i] = eau.angleOfVectors(v1,v2) * 180 / numpy.pi

    # Draw the raw data
    fig = plt.figure( fig.number )
    plt.clf()
    ax = fig.add_subplot(321)
    ax.plot( tripData[0:,0], tripData[0:,1], 'r.', alpha = 0.05 )
    ax.grid(True)
    ax = fig.add_subplot(322)
    ax.plot( numpy.arange(0,len(tripData)), tripData[0:,1], 'b.', alpha = 0.05 )
    ax.grid(True)
    ax = fig.add_subplot(323)
    ax.plot( tripData[0:,0], numpy.arange(0,len(tripData)), 'b.', alpha = 0.05 )
    ax.grid(True)
    ax = fig.add_subplot(324)
    ax.plot( numpy.arange(0,len(angles)), angles, 'r-' )
    ax.grid(True)
    ax = fig.add_subplot(325)
    ax.plot( numpy.arange(0,len(speedValues)), speedValues, 'g-' )
    ax.plot( numpy.arange(0,len(speedValues)), speedValues, 'g.', alpha=0.15 )
    ax.grid(True)
    ax = fig.add_subplot(326)
    ax.plot( numpy.arange(0,len(accelerationValues)), accelerationValues, 'b-' )
    ax.plot( numpy.arange(0,len(accelerationValues)), accelerationValues, 'b.', alpha=0.15 )
    ax.grid(True)

    return len(tripData)
    




# Read the trip files for the specified driver
driverFileName = os.path.join( driverTopDir, str(driverId)+".data")
file = open(driverFileName, 'rb')
trips=[]
while file.read(1):
    file.seek(-1,1)
    trips.append( pickle.load(file) )
file.close()

# The Raw Data Window
fig1 = plt.figure(num=1, figsize=(7,10.5), facecolor='lightblue')
plt.get_current_fig_manager().set_window_title("Raw Data")

# The Results Window (Pass 1)
fig2 = plt.figure(num=2, figsize=(7,10.5), facecolor='lightgray')
plt.get_current_fig_manager().set_window_title("Corrected Data Pass 1")

# The Results Window (Pass 2)
fig3 = plt.figure(num=3, figsize=(7,10.5), facecolor='lightgreen')
plt.get_current_fig_manager().set_window_title("Corrected Data Pass 2")

# Looping over the trips
for trip in trips:
    tripId = trip[0]
#    if tripId != 103: continue
    print( "Processing trip " + str(tripId) )
    tripData = trip[1]

    # Plot the original raw data
    nPoints = plotData( fig1, tripData )

    atext = "Driver " + str(driverId) + "  -  Trip " + str(tripId)
    fig1.text(0.05, 0.97, atext, horizontalalignment='left')
    m,s = divmod(nPoints-1,60)
    h,m = divmod(m,60)
    atext = "Duration: " + str("%dh %02dm %02ds"%(h,m,s))
    fig1.text(0.05, 0.94, atext, horizontalalignment='left')
    plt.draw()

    # First pass of corrections (gaps and speed jitters)
    tripData, nGaps, nJitterPoints = findMissingData( tripData )

    # Plot the corrected data
    plotData( fig2, tripData )
    atext = "Gaps : " + str(nGaps)
    fig2.text(0.05, 0.97, atext, horizontalalignment='left')
    atext = "Number of jitter points : " + str(nJitterPoints)
    fig2.text(0.05, 0.94, atext, horizontalalignment='left')
    plt.draw()

    # Second pass of corrections (removing zero speed segments)
    tripData, nZeroSpeedPoints = removeZeroSpeedSegments( tripData )
    fig=plt.figure(fig3.number)
    plotData( fig3, tripData )    
    plt.draw()
    

    # Show all the windows
    plt.show(block=False)
    input()

    

plt.close(fig1.number)
plt.close(fig2.number)
plt.close(fig3.number)
