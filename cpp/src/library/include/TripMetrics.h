#ifndef TRIPMETRICS_H
#define TRIPMETRICS_H

#include <iosfwd>

// Simple structure holding some key metrics of a trip
class TripMetrics
{
public:
    // Constructor
    TripMetrics();
    
    // Destructor
    ~TripMetrics();

    // driver id
    long driverId;

    // trip id
    long tripId;

    // The travel duration
    long travelDuration;
    
    // The travel length
    double travelLength;

    // The 25th percentile of the speed distribution
    double speed_p25;

    // The median the speed distribution
    double speed_p50;

    // The 75th percentile of the speed distribution
    double speed_p75;

    // The 95th percentile of the speed distribution
    double speed_p95;

    // The 5th percentile of the acceleration distribution
    double acceleration_p05;

    // The 25th percentile of the acceleration distribution
    double acceleration_p25;

    // The 75th percentile of the acceleration distribution
    double acceleration_p75;

    // The 95th percentile of the acceleration distribution
    double acceleration_p95;

    // The 5th percentile of the direction distribution
    double direction_p05;

    // The 25th percentile of the direction distribution
    double direction_p25;

    // The 75th percentile of the direction distribution
    double direction_p75;

    // The 95th percentile of the direction distribution
    double direction_p95;

    // The 5th percentile of the speedXacceleration distribution
    double speedXacceleration_p05;

    // The 25th percentile of the speedXacceleration distribution
    double speedXacceleration_p25;

    // The 75th percentile of the speedXacceleration distribution
    double speedXacceleration_p75;

    // The 95th percentile of the speedXacceleration distribution
    double speedXacceleration_p95;

    // The total negative turns (normalised to the trip length)
    double negativeTurns;

    // The total negative turns (normalised to the trip length)
    double positiveTurns;

    // Flag weather the trip contains 0 segments
    int zeroSegments;

    // Flag weather the trip contains less than 20 points after filtering
    int lessThan20Points;

    static std::ostream& variableNames( std::ostream& os );
};

// Overloading the output stream operator

std::ostream& operator<<( std::ostream& os,
			  const TripMetrics& metrics );

#endif
