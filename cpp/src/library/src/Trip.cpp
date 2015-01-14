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
    return *this;
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


const std::vector< Segment* >&
Trip::segments() const
{
    const_cast<Trip&>(*this).generateSegments();
    return m_segments;
}


Trip&
Trip::generateSegments()
{
    if ( m_segmentsGenerated ) return *this;

    m_extraTravelDuration = 0;
    m_extraTravelLength = 0;
    
    // First pass: correcting for missing segment paths
    // Look for single long jumps
    // Estimate the mean speed and add points along the line
    // Correct for jitter replacing the values of the data
    
    std::vector< std::pair< float, float > > rawData = m_rawData;
    
    std::vector< std::vector< std::pair< float, float > > > segmentsFirstPass;
    this->removeZeroSpeedSegments( rawData, segmentsFirstPass );

    // Second pass: identify zero velocity points.
    // Remove first and last zero speed points from the trip.
    std::vector< std::vector< std::pair< float, float > > > segmentsSecondPass;
    for ( std::vector< std::vector< std::pair< float, float > > >::const_iterator iSegment = segmentsFirstPass.begin();
         iSegment != segmentsFirstPass.end(); ++iSegment )
        this->identifyGapsCorrectJitter( *iSegment, segmentsSecondPass );
    segmentsFirstPass.clear();

    // Third pass: Remove angular jitter.
    std::vector< std::vector< std::pair< float, float > > > segmentsThirdPass;
    for ( std::vector< std::vector< std::pair< float, float > > >::const_iterator iSegment = segmentsSecondPass.begin();
         iSegment != segmentsSecondPass.end(); ++iSegment )
        this->removeAccuteAngleSegments( *iSegment, segmentsThirdPass );
    segmentsSecondPass.clear();
    
    // Now create the segment objects;
    for ( std::vector< std::vector< std::pair< float, float > > >::const_iterator iSegment = segmentsThirdPass.begin();
         iSegment != segmentsThirdPass.end(); ++iSegment )
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



Trip&
Trip::identifyGapsCorrectJitter( const std::vector< std::pair< float, float > >& tripData,
				 std::vector< std::vector< std::pair< float, float > > >& segments )
{
    const double maxAcceleration = 5; // The maximum acceleration allowed in a segment
    const double speedToTrigger = 10; // combined with a jump of 35 metres
    const unsigned int jitterTolerance = 1; // the number of seconds on the jitter
    
    double v_previous = 0;
    std::pair<float,float> p_previous = tripData[0];
    size_t i = 1;
    size_t segmentStartingIndex = 0;
    
    while ( i < tripData.size() ) {
        std::pair<float,float> p_current = tripData[i];
        double v_current = magnitude( p_current - p_previous );

        // Check for abrupt high speed and high acceleration in combination with no low speed.
        if ( ( std::abs( v_current - v_previous ) > maxAcceleration ) &&  v_current > speedToTrigger ) {

	    // Signal the end of the previous segment.
	    size_t nDiff = i - segmentStartingIndex;
	    if ( nDiff > 1 ) {
		std::vector< std::pair< float, float > > segment( tripData.begin() + segmentStartingIndex, tripData.begin() + i );
		segments.push_back( segment );
	    }
	    else {
		// Correct duration and length of trip for the skipped mini-segment.
		if ( i < tripData.size() - 1 ) {
		    std::pair<float,float> p_next = tripData[i];
		    double v_next = magnitude( p_next - p_current );
		    if ( std::abs( v_current - v_previous ) < maxAcceleration ) {
			m_extraTravelDuration += 1;
			m_extraTravelLength += magnitude( p_next - p_previous );
		    }
		}
	    }

	    // Check whether this is a jitter or a gap and adjust the starting index accordingly, as well as the correction to the travel length and time.
	    if ( i < tripData.size() - 1 ) {
		
		std::pair<float,float> p_next = tripData[i+1];
		double v_next = magnitude( p_next - p_current );
		if ( std::abs( v_current - v_next ) > maxAcceleration ) { // This is a gap
		    double averageSpeedInGap = 0.5 * ( v_next + v_previous );
		    double distanceSpentInGap = magnitude( p_current - p_previous );
		    m_extraTravelLength += distanceSpentInGap;
		    m_extraTravelDuration += static_cast<int>( std::nearbyint( distanceSpentInGap / averageSpeedInGap ) );
		    segmentStartingIndex = i;
		    ++i;
		    p_current = p_next;
		    v_current = v_next;
		}
		else { // This is a jitter. Check whether we have a sequence of spikes and skip them.
		    size_t j = i + 2;
		    bool endOfSpikesFound = false;
		    while ( j < tripData.size() - 1 ) {
			std::pair<float,float> p_nnext = tripData[j];
			double v_nnext = magnitude( p_next - p_nnext );
			std::pair<float,float> p_nnnext = tripData[j+1];
			double v_nnnext = magnitude( p_nnnext - p_nnext );
			
			if ( std::abs( v_nnnext - v_nnext ) < maxAcceleration ) { // End of spikes.
			    double averageSpeedInGap = 0.5 * ( v_nnext + v_previous );
			    double distanceSpentInGap = magnitude( p_next - p_previous );
			    m_extraTravelLength += distanceSpentInGap;
			    m_extraTravelDuration += static_cast<int>( std::nearbyint( distanceSpentInGap / averageSpeedInGap ) );
			    i = j;
			    segmentStartingIndex = i-1;
			    p_current = p_next;
			    v_current = averageSpeedInGap;
			    endOfSpikesFound = true;
			    break;
			}
			++j;
			p_next = p_nnext;
		    }
		    if ( ! endOfSpikesFound ) {
			i = j;
			segmentStartingIndex = i;
		    }
		}
	    }
        }

        p_previous = p_current;
        v_previous = v_current;
        ++i;
    }

    
    if ( tripData.size() - segmentStartingIndex > 2 ) {
        std::vector< std::pair< float, float > > segment( tripData.begin()+segmentStartingIndex, tripData.end() );
        segments.push_back( segment );
    }
    
    return *this;
}



static double
angleAmongVectors( const std::pair<float,float>& v1,
		   const std::pair<float,float>& v2 )
{
    const double pi = std::atan( 1.0 ) * 4;
    double mv1 = std::sqrt( std::pow(v1.first,2) + std::pow(v1.second, 2) );
    if (mv1 == 0 ) return 0;
    double mv2 = std::sqrt( std::pow(v2.first,2) + std::pow(v2.second, 2) );
    if (mv2 == 0 ) return 0;
    double mvv = mv1 * mv2;
    double sint = ( v1.first * v2.second - v1.second * v2.first ) / mvv;
    double cost = ( v1.first * v2.first + v1.second * v2.second ) / mvv;
    if ( sint > 1 ) sint = 1;
    if ( sint < -1 ) sint = -1;
    if (cost >= 0  )
	return std::asin( sint );
    else {
	if (sint > 0 )
            return pi - std::asin( sint );
	else {
	    if ( cost > 1 ) cost = 1;
	    if ( cost < -1 ) cost = -1;
	    return - std::acos( cost );
	}
    }
}


Trip&
Trip::removeAccuteAngleSegments( const std::vector< std::pair< float, float > >& tripData,
				 std::vector< std::vector< std::pair< float, float > > >& segments )
{
    const double pi = std::atan( 1.0 ) * 4;
    const double maxAngle = 100 * pi / 180.0; // 100 degrees turn in a second!

    size_t segentStartingIndex = 0;

    std::pair<float, float> v_previous = tripData[1]-tripData[0];
    size_t i = 2;
    while ( i < tripData.size() ) {
	std::pair<float, float> v_current = tripData[i] - tripData[i-1];
	double angle = angleAmongVectors( v_current, v_previous );
	
        if ( std::abs( angle ) > maxAngle ) {
	    if ( i - segentStartingIndex > 2 ) {
		std::vector< std::pair< float, float > > segment( tripData.begin() + segentStartingIndex, tripData.begin() + i - 1 );
		segments.push_back( segment );
	    }
	    
	    m_extraTravelDuration += 2;
	    m_extraTravelLength += magnitude(tripData[i-2] - tripData[i]);
	    
            segentStartingIndex = i;
	    i += 2;
	    if ( i - 1 < tripData.size() )
		v_previous = tripData[i-1] - tripData[i-2];
	    continue;
        }
        v_previous = v_current;
	++i;
    }
    
    if ( tripData.size() - segentStartingIndex > 2 ) {
        std::vector< std::pair< float, float > > segment( tripData.begin()+segentStartingIndex, tripData.end() );
        segments.push_back( segment );
    }

    return *this;
}



Trip&
Trip::removeZeroSpeedSegments( const std::vector< std::pair< float, float > >& tripData,
			       std::vector< std::vector< std::pair< float, float > > >& segments )
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
    
    if ( tripData.size() - segentStartingIndex > 2 ) {
        std::vector< std::pair< float, float > > segment( tripData.begin()+segentStartingIndex, tripData.end() );
        segments.push_back( segment );
    }

    return *this;
}

