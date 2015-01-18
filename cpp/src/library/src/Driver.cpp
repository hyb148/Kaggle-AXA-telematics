#include "Driver.h"

Driver::Driver( int driverId ):
  m_driverId( driverId ),
  m_trips()
{}

Driver::~Driver()
{}

Driver&
Driver::loadTripData( const std::vector< std::pair< int, std::vector< std::pair<float,float> > > >& tripData )
{
    m_trips.clear();
    m_trips.reserve( tripData.size() );
    for ( std::vector< std::pair< int, std::vector< std::pair<float,float> > > >::const_iterator iTrip = tripData.begin();
         iTrip != tripData.end(); ++iTrip ) {
        m_trips.push_back( Trip( iTrip->first ) );
        m_trips.back().setTripData( iTrip->second );
    }
    
    return *this;
}

std::vector< TripMetrics >
Driver::tripMetrics() const
{
    std::vector< TripMetrics > localMetrics;
    localMetrics.reserve(200);
    for ( std::vector< Trip >::const_iterator iTrip = m_trips.begin();
	  iTrip != m_trips.end(); ++iTrip ) {
	TripMetrics m = iTrip->metrics();
	m.driverId = m_driverId;
	localMetrics.push_back( m );
    }
    return localMetrics;
}
