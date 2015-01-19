#include "TripMetrics.h"
#include <ostream>
#include <cmath>

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

std::vector<double>
TripMetrics::values() const
{
    std::vector<double> result;
    result.reserve( 20 );
    
    result.push_back( travelDuration );
    result.push_back( travelLength );
    result.push_back( speed_p25 );
    result.push_back( speed_p50 );
    result.push_back( speed_p75 );
    result.push_back( speed_p95 );
    result.push_back( acceleration_p05 );
    result.push_back( acceleration_p25 );
    result.push_back( acceleration_p75 );
    result.push_back( acceleration_p95 );
    result.push_back( direction_p05 );
    result.push_back( direction_p25 );
    result.push_back( direction_p75 );
    result.push_back( direction_p95 );
    result.push_back( speedXacceleration_p05 );
    result.push_back( speedXacceleration_p25 );
    result.push_back( speedXacceleration_p75 );
    result.push_back( speedXacceleration_p95 );
    result.push_back( negativeTurns );
    result.push_back( positiveTurns );

    return result;
}



std::vector< std::function<bool(double) > >
TripMetrics::validityChecks()
{
    std::vector< std::function<bool(double)> > result;
    result.reserve(20);
    result.push_back( [](double x){ return (1+x>0);} ); // travelDuration
    result.push_back( [](double x){ return (1+x>0);} ); // travelLength
    result.push_back( [](double x){ return (1+x>0);} ); // speed_p25
    result.push_back( [](double x){ return (1+x>0);} ); // speed_p50
    result.push_back( [](double x){ return (1+x>0);} ); // speed_p75
    result.push_back( [](double x){ return (1+x>0);} ); // speed_p95
    result.push_back( [](double x){ return (x<0);} ); // acceleration_p05
    result.push_back( [](double x){ return (x<0);} ); // acceleration_p25
    result.push_back( [](double x){ return (x>0);} ); // acceleration_p75
    result.push_back( [](double x){ return (x>0);}  ); // acceleration_p95
    result.push_back( [](double x){ return (x<0);}  ); // direction_p05
    result.push_back( [](double x){ return (x<0);}  ); // direction_p25
    result.push_back( [](double x){ return (x>0);}  ); // direction_p75
    result.push_back( [](double x){ return (x>0);}  ); // direction_p95
    result.push_back( [](double x){ return (x<0);}  ); // speedXacceleration_p05
    result.push_back( [](double x){ return (x<0);}  ); // speedXacceleration_p25
    result.push_back( [](double x){ return (x>0);}  ); // speedXacceleration_p75
    result.push_back( [](double x){ return (x>0);}  ); // speedXacceleration_p95
    result.push_back( [](double x){ return (0.001+x>0);}  ); // negativeTurns
    result.push_back( [](double x){ return (0.001+x>0);}  ); // positiveTurns

    return result;
}


std::vector< std::function<double(double)> >
TripMetrics::transformations()
{
    std::vector< std::function<double(double)> > result;

    result.reserve(20);
    result.push_back( [](double x){ return std::log10(1+x);} ); // travelDuration
    result.push_back( [](double x){ return std::log10(1+x);} ); // travelLength
    result.push_back( [](double x){ return std::log10(1+x);} ); // speed_p25
    result.push_back( [](double x){ return std::log10(1+x);} ); // speed_p50
    result.push_back( [](double x){ return std::log10(1+x);} ); // speed_p75
    result.push_back( [](double x){ return std::log10(1+x);} ); // speed_p95
    result.push_back( [](double x){ return std::log10(-x);} ); // acceleration_p05
    result.push_back( [](double x){ return std::log10(-x);} ); // acceleration_p25
    result.push_back( [](double x){ return std::log10(x);} ); // acceleration_p75
    result.push_back( [](double x){ return std::log10(x);} ); // acceleration_p95
    result.push_back( [](double x){ return std::log10(-x);} ); // direction_p05
    result.push_back( [](double x){ return std::log10(-x);} ); // direction_p25
    result.push_back( [](double x){ return std::log10(x);} ); // direction_p75
    result.push_back( [](double x){ return std::log10(x);} ); // direction_p95
    result.push_back( [](double x){ return std::log10(-x);} ); // speedXacceleration_p05
    result.push_back( [](double x){ return std::log10(-x);} ); // speedXacceleration_p25
    result.push_back( [](double x){ return std::log10(x);} ); // speedXacceleration_p75
    result.push_back( [](double x){ return std::log10(x);} ); // speedXacceleration_p95
    result.push_back( [](double x){ return std::log10(0.001+x);} ); // negativeTurns
    result.push_back( [](double x){ return std::log10(0.001+x);} ); // positiveTurns

    return result;
}
