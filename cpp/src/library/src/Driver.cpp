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
