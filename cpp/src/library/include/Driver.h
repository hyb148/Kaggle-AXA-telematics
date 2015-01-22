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
    
    // Returns the trip objects
    inline const std::vector< Trip >& trips() const { return m_trips; }

    // Returns the trip metrics
    std::vector< TripMetrics > tripMetrics() const;

    // Operator for searching in a vector
    inline bool operator==( const Driver& rhs ) const { return this->id() == rhs.id(); }
    inline bool operator==( int rhs ) const { return this->id() == rhs; }
    
private:
    // The trip id
    int m_driverId;
    
    // The trip objects
    std::vector< Trip > m_trips;
    
};

#endif
