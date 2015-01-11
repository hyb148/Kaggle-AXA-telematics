#include "Trip.h"
#include "Segment.h"
#include <cmath>

Trip::Trip( int tripId):
  m_tripId( tripId ),
  m_rawData(),
  m_segments(),
  m_segmentsGenerated( false ),
  m_extraTravelDuration(0),
  m_extraTravelLength(0),
  m_distanceOfEndPoint(0)
{
}

Trip::~Trip()
{
    for ( std::vector< Segment* >::iterator i = m_segments.begin();
         i != m_segments.end(); ++i )
        delete *i;
}

Trip&
Trip::setTripData( const std::vector< std::pair<float,float> >& data )
{
    m_rawData = data;
    if ( m_segmentsGenerated ) {
        for ( std::vector< Segment* >::iterator i = m_segments.begin();
             i != m_segments.end(); ++i )
            delete *i;
        m_segments.clear();
        m_segmentsGenerated = false;
        m_extraTravelDuration = 0;
        m_extraTravelLength = 0;
    }
    
    const std::pair<float,float>& endPoint = data.back();
    m_distanceOfEndPoint = std::sqrt( std::pow(endPoint.first, 2) + std::pow(endPoint.second,2) );
    //return *this;
    return this->generateSegments();
}


long
Trip::travelDuration() const
{
    const_cast<Trip&>(*this).generateSegments();
    long duration = 0;
    for ( std::vector< Segment* >::const_iterator iSegment = m_segments.begin();
         iSegment != m_segments.end(); ++iSegment )
        duration += (*iSegment)->travelDuration();
    return duration + m_extraTravelDuration;
}

double
Trip::travelLength() const
{
    const_cast<Trip&>(*this).generateSegments();
    double length = 0;
    for ( std::vector< Segment* >::const_iterator iSegment = m_segments.begin();
         iSegment != m_segments.end(); ++iSegment )
        length += (*iSegment)->travelLength();
    
    return length + m_extraTravelLength;
}


long
Trip::numberOfSegments() const
{
    const_cast<Trip&>(*this).generateSegments();
    return m_segments.size();
}


Trip&
Trip::generateSegments()
{
    if ( m_segmentsGenerated ) return *this;
    
    // First pass: correcting for missing segment paths
    // Look for single long jumps
    // Estimate the mean speed and add points along the line
    // Correct for jitter replacing the values of the data
    std::vector< std::vector< std::pair< float, float > > > segmentsFirstPass = this->identifyGapsCorrectJitter();

    // Second pass: identify zero velocity points.
    // Mark first and last point of each segment of non-zero speed, after smoothing...
    // Remove first and last zero speed points from the trip.
    std::vector< std::vector< std::pair< float, float > > > segmentsSecondPass;
    for ( std::vector< std::vector< std::pair< float, float > > >::const_iterator iSegment = segmentsFirstPass.begin();
         iSegment != segmentsFirstPass.end(); ++iSegment )
        this->removeZeroSpeedSegments( *iSegment, segmentsSecondPass );
    segmentsFirstPass.clear();

    // Third pass: Remove / correct angular jitter.
    
    // Now create the segment objects;
    for ( std::vector< std::vector< std::pair< float, float > > >::const_iterator iSegment = segmentsSecondPass.begin();
         iSegment != segmentsSecondPass.end(); ++iSegment )
        m_segments.push_back( new Segment( *iSegment ) );
    
    m_segmentsGenerated = true;
    return *this;
}


static double
innerProduct( const std::pair<float,float>& v1,
            const std::pair<float,float>& v2 )
{
    return v1.first * v2.first + v1.second * v2.second;
}


static double
magnitude( const std::pair<float,float>& v )
{
    return std::sqrt( innerProduct(v,v) );
}

static std::pair<float,float>
operator- ( const std::pair<float,float>& v1,
           const std::pair<float,float>& v2 )
{
    return std::make_pair(v1.first - v2.first, v1.second - v2.second );
}

static std::pair<float,float>
operator+ ( const std::pair<float,float>& v1,
           const std::pair<float,float>& v2 )
{
    return std::make_pair(v1.first + v2.first, v1.second + v2.second );
}

static std::pair<float,float>
operator*( double f, const std::pair<float,float>& v )
{
    return std::make_pair(v.first * f, v.second * f );
}

static std::pair<float,float>
operator*( const std::pair<float,float>& v, double f )
{
    return std::make_pair(v.first * f, v.second * f );
}

static std::pair<float,float>
operator/( const std::pair<float,float>& v, double f )
{
    return std::make_pair(v.first / f, v.second / f );
}


std::vector< std::vector< std::pair< float, float > > >
Trip::identifyGapsCorrectJitter()
{
    const double maxAcceleration = 5; // The maximum acceleration allowed in a segment
    const double speedToTrigger = 10; // A jump of ~50 metres
    const unsigned int jitterTolerance = 1; // the number of seconds on the jitter
    
    m_extraTravelDuration = 0;
    m_extraTravelLength = 0;
    
    double v_previous = 0;
    std::pair<float,float> p_previous = m_rawData[0];
    size_t i = 0;
    
    std::vector< size_t > gapIndices;
    
    while ( i < m_rawData.size() - 1 ) {
        std::pair<float,float> p_current = m_rawData[i];
        double v_current = magnitude( p_current - p_previous );


        // Check for abrupt high speed and high acceleration followed by normal speed
        if ( ( std::abs( v_current - v_previous ) > maxAcceleration ) &&  v_current > speedToTrigger ) {
            std::pair<float,float> p_next = m_rawData[i + 1];
            double v_next = magnitude( p_next - p_current );
            
            if ( std::abs( v_current - v_next ) > maxAcceleration ) {  // We may need to relax this in order to allow for smaller decceleration values
                double averageSpeed = 0.5 * (v_previous + v_next);
                if (averageSpeed == 0 ) { // This is a jitter
                    m_rawData[i] = 0.5 * (p_next - p_previous);
                    v_previous = magnitude( 0.5 * (p_next - p_previous) );
                    p_previous = p_next;
                    i += 2;
                    continue;
                }
                int secondsInGap = int( std::nearbyint( v_current / averageSpeed ) );
                if ( secondsInGap > jitterTolerance ) {  // This is a gap
                    gapIndices.push_back( i );
                    i += 3;
                    m_extraTravelDuration += secondsInGap;
                    m_extraTravelLength += v_current;
                    
                    if (i < m_rawData.size() ) {
                        p_previous = m_rawData[i-1];
                        v_previous = magnitude( m_rawData[i-1] - m_rawData[i-2] );
                    }
                    continue;
                }
                else { // This is a jitter again. We need to correct the values
                    m_rawData[i] = 0.5 * (p_next - p_previous);
                    v_previous = magnitude( 0.5 * (p_next - p_previous) );
                    p_previous = p_next;
                    i += 2;
                    continue;
                }
            }
        }

        p_previous = p_current;
        v_previous = v_current;
        ++i;
    }
    
    std::vector< std::vector< std::pair< float, float > > > segments;
    if ( gapIndices.empty() ) {
        segments.push_back( m_rawData );
        return segments;
    }
    
    i = 0;
    for (size_t m = 0; m < gapIndices.size(); ++m ) {
        std::vector< std::pair<float,float> > segment( m_rawData.begin() + i, m_rawData.begin() + gapIndices[m] );
        if (segment.size() > 2 ) segments.push_back( segment );
        i = gapIndices[m] + 1;
    }

    std::vector< std::pair<float,float> > segment( m_rawData.begin() + i, m_rawData.end() );
    if (segment.size() > 2 ) segments.push_back( segment );
    
    return segments;
}

const Trip&
Trip::removeZeroSpeedSegments( const std::vector< std::pair< float, float > >& tripData,
                              std::vector< std::vector< std::pair< float, float > > >& segments ) const
{
    const double zeroSpeedTolerance = 1.5;
    
    size_t segentStartingIndex = 0;
    size_t zeroSpeedCounter = 0;
    std::pair<float, float> p_previous = tripData[0];
    for (size_t i = 1; i < tripData.size(); ++i ) {
        std::pair<float, float> p_current = tripData[i];
        double v_current = magnitude( p_current - p_previous );

        if ( v_current < zeroSpeedTolerance ) {
            if ( zeroSpeedCounter == 0 ) { // Mark the end of a segment and beginning of a zero speed sequence
                if ( i - segentStartingIndex > 1 ) {
                    std::vector< std::pair< float, float > > segment( tripData.begin() + segentStartingIndex, tripData.begin() + i );
                    segments.push_back( segment );
                }
            }
            zeroSpeedCounter += 1;
            segentStartingIndex = i;
        }
        else {
            zeroSpeedCounter = 0;
        }
        p_previous = p_current;
    }

    
    if ( tripData.size() - segentStartingIndex > 1 ) {
        std::vector< std::pair< float, float > > segment( tripData.begin()+segentStartingIndex, tripData.end() );
        segments.push_back( segment );
    }

    return *this;
}

