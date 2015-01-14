#ifndef TRIP_H
#define TRIP_H

#include <string>
#include <vector>
#include <utility>

class Segment;

class Trip {
public:
    // Constructor
    explicit Trip( int tripId = 0 );
    
    // Destructor
    virtual ~Trip();
    
    // Returns the trip id
    inline int id() const { return m_tripId; }
    
    // Sets the trip data
    Trip& setTripData( const std::vector< std::pair<float,float> >& data );
    
    // Returns the travel duration
    long travelDuration() const;
    
    // Returns the travel length
    double travelLength() const;
    
    // Returns the distance of the end point
    inline double distanceOfEndPoint() const { return m_distanceOfEndPoint; }
    
    // Returns the number of segments
    long numberOfSegments() const;
    
    // Returns the raw data
    inline const std::vector< std::pair< float, float > >& rawData() const { return m_rawData; }
    
    // Returns the segments
    const std::vector< Segment* >& segments() const;
    
    // Operator for searching in a vector
    inline bool operator==( const Trip& rhs ) const { return this->id() == rhs.id(); }
    inline bool operator==( int rhs ) const { return this->id() == rhs; }

private:
    // The trip id
    int m_tripId;
    
    // The raw data
    std::vector< std::pair< float, float > > m_rawData;
    
    // The trip segments
    std::vector< Segment* > m_segments;
    
    // Flags if segments have been generated
    bool m_segmentsGenerated;
    
    // The travel duration and length not included in the segments
    long m_extraTravelDuration;
    double m_extraTravelLength;
    
    // The distance of the end point
    double m_distanceOfEndPoint;
    
    
private: // Private methods
    Trip& generateSegments();
    
    // Identifies the gaps and corrects the jitter generating segment data
    Trip& identifyGapsCorrectJitter( const std::vector< std::pair< float, float > >& rawData,
				     std::vector< std::vector< std::pair< float, float > > >& segments );
    
    // Removes the zero speed segments
    Trip& removeZeroSpeedSegments( const std::vector< std::pair< float, float > >& tripData,
				   std::vector< std::vector< std::pair< float, float > > >& segments );
    
    // Removes the segments with spurious angles
    Trip& removeAccuteAngleSegments( const std::vector< std::pair< float, float > >& tripData,
				     std::vector< std::vector< std::pair< float, float > > >& segments );
};

#endif
