#include "TripMetrics.h"
#include <ostream>

TripMetrics::TripMetrics():
    driverId(0),
    tripId(0),
    travelDuration(0),
    travelLength(0),
    lengthToDistance(0),
    speed_p25(0),
    speed_q13(0),
    speed_m(0),
    speed_p95(0),
    acceleration_p05(0),
    acceleration_p25(0),
    acceleration_q13(0),
    acceleration_m(0),
    acceleration_p95(0),
    direction_p05(0),
    direction_p25(0),
    direction_q13(0),
    direction_m(0),
    direction_p95(0),
    r2_sa(0),
    r2_sd(0),
    r2_ad(0)
{}

TripMetrics::~TripMetrics()
{}


std::ostream&
operator<<( std::ostream& os,
	    const TripMetrics& metrics )
{
    os << metrics.driverId << " " << metrics.tripId << " " <<
	metrics.travelDuration << " " <<
	metrics.travelLength <<  " " <<
	metrics.lengthToDistance << " " <<
	metrics.speed_p25 << " " <<
	metrics.speed_q13 << " " <<
	metrics.speed_m << " " <<
	metrics.speed_p95 << " " <<
	metrics.acceleration_p05 << " " <<
	metrics.acceleration_p25 << " " <<
	metrics.acceleration_q13 << " " <<
	metrics.acceleration_m << " " <<
	metrics.acceleration_p95 << " " <<
	metrics.direction_p05 << " " <<
	metrics.direction_p25 << " " <<
	metrics.direction_q13 << " " <<
	metrics.direction_m << " " <<
	metrics.direction_p95 << " " <<
	metrics.r2_sa << " " <<
	metrics.r2_sd << " " <<
	metrics.r2_ad;
    
    return os;
}
