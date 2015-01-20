#ifndef TRIP_H
#define TRIP_H

#include <string>
#include <vector>
#include <utility>
#include <tuple>

class Segment;

#include "TripMetrics.h"

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

    // Returns the metrics of the trip
    TripMetrics metrics() const;
    
    // Returns the travel duration
    long travelDuration() const;
    
    // Returns the travel length
    double travelLength() const;
    
    // Returns the distance of the end point
    inline double distanceOfEndPoint() const { return m_distanceOfEndPoint; }
    
    // Returns the number of segments
    long numberOfSegments() const;

    // Returns the speed values
    std::vector<double> speedValues() const;

    // Returns the acceleration values
    std::vector<double> accelerationValues() const;

    // Returns the direction (angular) values
    std::vector<double> directionValues() const;

    // Returns the speed, acceleration and direction values as an assosiation
    std::vector< std::tuple<double,double,double> > speedAccelerationDirectionValues() const;

    // Returns the 5th, 25th, 50th, 75th and 95th quantile of the speed distribution
    std::vector< double > speedQuantiles() const;

    // Returns the 5th, 25th, 50th, 75th and 95th quantile of the acceleration distribution
    std::vector< double > accelerationQuantiles() const;
    
    // Returns the 5th, 25th, 50th, 75th and 95th quantile of the direction distribution
    std::vector< double > directionQuantiles() const;

    // Returns the averaged out FFT transformation of the speed values
    std::vector< double > rollingFFT( long sampleSize = 20 ) const;
    
    // Returns the averaged out FFT transformation of the direction values
    std::vector< double > rollingFFT_direction( long sampleSize = 20 ) const;
    
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
    
    // Private methods. This is for segment generation
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
