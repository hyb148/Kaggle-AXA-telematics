#ifndef TRIPMETRICS_H
#define TRIPMETRICS_H

#include <iosfwd>
#include <vector>
#include <string>
#include <valarray>

// Simple structure holding some key metrics of a trip
class TripMetrics
{
public:
    // Constructor
    TripMetrics( long tripId,
                 const std::vector<double>& values );
    
    // Destructor
    ~TripMetrics();

    // Returns the driver id
    inline long driverId() const { return m_driverId; }
    // Sets the driver id
    inline TripMetrics& setDriverId( long driverId ) { m_driverId = driverId; return *this; }

    // Returns the tripId
    inline long tripId() const { return m_tripId; }
    
    // The values
    inline const std::vector<double>& values() const { return m_values; }
    
    // Returns a vector of the metric descriptions
    const std::vector< std::string >& descriptions() const;
    
    // Writes the descriptions to an output stream (space separated values)
    std::ostream& writeDescriptions( std::ostream& out ) const;
    
    // Writes the values to an output stream (space separated values)
    std::ostream& writeValues( std::ostream& out ) const;
    
 private:
    long m_tripId;
    long m_driverId;
    std::vector<double> m_values;
    
};

// Overloading the output stream operator
std::ostream& operator<<( std::ostream& os,
			  const TripMetrics& metrics );

#endif
