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
    std::vector< std::vector< std::pair< float, float > > > identifyGapsCorrectJitter();
    
    // Removes the zero speed segments
    const Trip& removeZeroSpeedSegments( const std::vector< std::pair< float, float > >& tripData,
                                        std::vector< std::vector< std::pair< float, float > > >& segments ) const;
};

#endif
