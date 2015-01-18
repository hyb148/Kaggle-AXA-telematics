#include "TripMetrics.h"
#include <ostream>

TripMetrics::TripMetrics():
    driverId(0),
    tripId(0),
    travelDuration(0),
    travelLength(0),
    speed_p25(0),
    speed_p50(0),
    speed_p75(0),
    speed_p95(0),
    acceleration_p05(0),
    acceleration_p25(0),
    acceleration_p75(0),
    acceleration_p95(0),
    direction_p05(0),
    direction_p25(0),
    direction_p75(0),
    direction_p95(0),
    speedXacceleration_p05(0),
    speedXacceleration_p25(0),
    speedXacceleration_p75(0),
    speedXacceleration_p95(0),
    negativeTurns(0),
    positiveTurns(0),
    zeroSegments(0),
    lessThan20Points(0)
{}

TripMetrics::~TripMetrics()
{}

std::ostream&
TripMetrics::variableNames( std::ostream& os )
{
    os << "driverId "
       << "tripId "
       << "travelDuration "
       << "travelLength "
       << "speed_p25 "
       << "speed_p50 "
       << "speed_p75 "
       << "speed_p95 "
       << "acceleration_p05 "
       << "acceleration_p25 "
       << "acceleration_p75 "
       << "acceleration_p95 "
       << "direction_p05 "
       << "direction_p25 "
       << "direction_p75 "
       << "direction_p95 "
       << "speedXacceleration_p05 "
       << "speedXacceleration_p25 "
       << "speedXacceleration_p75 "
       << "speedXacceleration_p95 "
       << "negativeTurns "
       << "positiveTurns "
       << "zeroSegments "
       << "lessThan20Points";
    return os;
}


std::ostream&
operator<<( std::ostream& os,
	    const TripMetrics& metrics )
{
    os << metrics.driverId << " " << metrics.tripId << " " <<
	metrics.travelDuration << " " <<
	metrics.travelLength <<  " " <<
	metrics.speed_p25 << " " <<
	metrics.speed_p50 << " " <<
	metrics.speed_p75 << " " <<
	metrics.speed_p95 << " " <<
	metrics.acceleration_p05 << " " <<
	metrics.acceleration_p25 << " " <<
	metrics.acceleration_p75 << " " <<
	metrics.acceleration_p95 << " " <<
	metrics.direction_p05 << " " <<
	metrics.direction_p25 << " " <<
	metrics.direction_p75 << " " <<
	metrics.direction_p95 << " " <<
	metrics.speedXacceleration_p05 << " " <<
	metrics.speedXacceleration_p25 << " " <<
	metrics.speedXacceleration_p75 << " " <<
	metrics.speedXacceleration_p95 << " " <<
	metrics.negativeTurns << " " <<
	metrics.positiveTurns << " " <<
	metrics.zeroSegments << " " <<
	metrics.lessThan20Points;
    
    return os;
}
