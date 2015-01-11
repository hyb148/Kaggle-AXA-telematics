#ifndef DRIVER_H
#define DRIVER_H

#include <string>
#include "Trip.h"

class Driver {
public:
    // Constructor
    explicit Driver( int driverId = 0 );
    
    // Destructor
    virtual ~Driver();
    
    // Returns the trip id
    inline int id() const { return m_driverId; }
    
    // Creates trip objects from data
    Driver& loadTripData( const std::vector< std::pair< int, std::vector< std::pair<float,float> > > >& tripData );
    
private:
    // The trip id
    int m_driverId;
    
    // The trip objects
    std::vector< Trip > m_trips;
    
};

#endif
