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

    // The ratio of the travel length to the distance of the end point
    double lengthToDistance;

    // The 25th percentile of the speed distribution
    double speed_p25;

    // The inter-quantile distance of the speed distribution
    double speed_q13;
    
    // The median the speed distribution
    double speed_m;

    // The 95th percentile of the speed distribution
    double speed_p95;

    // The 5th percentile of the acceleration distribution
    double acceleration_p05;

    // The 25th percentile of the acceleration distribution
    double acceleration_p25;

    // The inter-quantile distance of the acceleration distribution
    double acceleration_q13;

    // The median of the acceleration distribution
    double acceleration_m;
    
    // The 95th percentile of the acceleration distribution
    double acceleration_p95;

    // The 5th percentile of the direction distribution
    double direction_p05;

    // The 25th percentile of the direction distribution
    double direction_p25;

    // The inter-quantile distance of the direction distribution
    double direction_q13;

    // The median of the direction distribution
    double direction_m;
    
    // The 95th percentile of the direction distribution
    double direction_p95;

    // The R^2 value of the speed to acceleration correlation
    double r2_sa;

    // The R^2 value of the speed to direction correlation
    double r2_sd;

    // The R^2 value of the acceleration to direction correlation
    double r2_ad;
};

// Overloading the output stream operator

std::ostream& operator<<( std::ostream& os,
			  const TripMetrics& metrics );

#endif
