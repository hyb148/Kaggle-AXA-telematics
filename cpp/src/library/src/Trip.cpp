#include "Trip.h"
#include "Segment.h"
#include <cmath>
#include <algorithm>


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


static std::vector< double >
findQuantiles( std::vector<double>& values )
{
    if (values.size() == 0 )
	return std::vector<double>();
    
    size_t Q05 = ( values.size() *  5 ) / 100;
    size_t Q25 = ( values.size() * 25 ) / 100;
    size_t Q50 = ( values.size() * 50 ) / 100;
    size_t Q75 = ( values.size() * 75 ) / 100;
    size_t Q95 = ( values.size() * 95 ) / 100;

    std::nth_element(values.begin()          , values.begin() + Q05, values.end() );
    std::nth_element(values.begin() + Q05 + 1, values.begin() + Q25, values.end() );
    std::nth_element(values.begin() + Q25 + 1, values.begin() + Q50, values.end() );
    std::nth_element(values.begin() + Q50 + 1, values.begin() + Q75, values.end() );
    std::nth_element(values.begin() + Q75 + 1, values.begin() + Q95, values.end() );

    return std::vector<double> ( {
	        *(values.begin() + Q05),
		*(values.begin() + Q25),
		*(values.begin() + Q50),
		*(values.begin() + Q75),
		*(values.begin() + Q95) } );
}


static double
r2_correlation( const std::vector<double>& x,
		const std::vector<double>& y )
{
    double Sx = 0;
    double Sy = 0;
    double Sxy = 0;
    double Sxx = 0;

    size_t n = x.size();

    for ( size_t i = 0; i < n; ++i ) {
	Sx += x[i];
	Sy += y[i];
	Sxy += x[i] * y[i];
	Sxx += x[i] * x[i];
    }
    double Yb = Sy / n;
    double Xb = Sx / n;
    
    double slope = ( n * Sxy - Sx * Sy ) / ( n * Sxx - Sx * Sx);
    double y_intercept = Yb - slope * Xb;

    double S_Yres = 0;
    double Sres = 0;
    
    for ( size_t i = 0; i < n; ++i ) {
	S_Yres += std::pow( y[i] - y_intercept - slope * x[i], 2 );
	Sres += std::pow( y[i] - Yb, 2);
    }
    
    return (Sres - S_Yres ) / Sres ;
}


TripMetrics
Trip::metrics() const
{
    TripMetrics metrics;
    const_cast<Trip&>(*this).generateSegments();

    metrics.tripId = m_tripId;
    metrics.travelDuration = this->travelDuration();
    metrics.travelLength = this->travelLength();
    if ( m_distanceOfEndPoint > 0 )
	metrics.lengthToDistance = metrics.travelLength / m_distanceOfEndPoint;

    std::vector<double> speedValues;
    speedValues.reserve( m_rawData.size() - 2 );
    std::vector<double> accelerationValues;
    accelerationValues.reserve( m_rawData.size() - 2 );
    std::vector<double> directionValues;
    directionValues.reserve( m_rawData.size() - 2 );

    for ( std::vector< Segment* >::const_iterator iSegment = m_segments.begin();
	  iSegment != m_segments.end(); ++iSegment ) {
	std::vector< std::tuple<double,double,double> > segmentValues = (*iSegment)->speedAccelerationDirectionValues();
	for ( std::vector< std::tuple<double,double,double> >::const_iterator iValue = segmentValues.begin();
	      iValue != segmentValues.end(); ++iValue ) {
	    speedValues.push_back( std::get<0>( *iValue ) );
	    accelerationValues.push_back( std::get<1>( *iValue ) );
	    directionValues.push_back( std::get<2>( *iValue ) );
	}
    }

    if ( speedValues.size() < 3 ) return metrics;

    metrics.r2_sa = r2_correlation( speedValues, accelerationValues );
    metrics.r2_sd = r2_correlation( speedValues, directionValues );
    metrics.r2_ad = r2_correlation( accelerationValues, directionValues );
    
    // Calculate the percentiles.
    std::vector<double> speedPercentiles = findQuantiles( speedValues );
    metrics.speed_p25 = speedPercentiles[1];
    metrics.speed_q13 = speedPercentiles[3] - speedPercentiles[1];
    metrics.speed_m = speedPercentiles[2];
    metrics.speed_p95 = speedPercentiles[4];
    std::vector<double> accelerationPercentiles = findQuantiles( accelerationValues );
    metrics.acceleration_p05 = accelerationPercentiles[0];
    metrics.acceleration_p25 = accelerationPercentiles[1];
    metrics.acceleration_q13 = accelerationPercentiles[3] - accelerationPercentiles[1];
    metrics.acceleration_m = accelerationPercentiles[2];
    metrics.acceleration_p95 = accelerationPercentiles[4];
    std::vector<double> directionPercentiles = findQuantiles( directionValues );
    metrics.direction_p05 = directionPercentiles[0];
    metrics.direction_p25 = directionPercentiles[1];
    metrics.direction_q13 = directionPercentiles[3] - directionPercentiles[1];
    metrics.direction_m = directionPercentiles[2];
    metrics.direction_p95 = directionPercentiles[4];

    return metrics;
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
Trip::travelDuration() const
{
    const_cast<Trip&>(*this).generateSegments();
    long duration = 0;
    for ( std::vector< Segment* >::const_iterator iSegment = m_segments.begin();
         iSegment != m_segments.end(); ++iSegment )
        duration += (*iSegment)->travelDuration();
    
    return duration + m_extraTravelDuration;
}



std::vector<double>
Trip::speedValues() const
{
    const_cast<Trip&>(*this).generateSegments();
    std::vector<double> speedValues;
    speedValues.reserve( m_rawData.size() - 1 );

    for ( std::vector< Segment* >::const_iterator iSegment = m_segments.begin();
	  iSegment != m_segments.end(); ++iSegment ) {
	std::vector<double> segmentValues = (*iSegment)->speedValues();
	for ( std::vector<double>::const_iterator iValue = segmentValues.begin();
	      iValue != segmentValues.end(); ++iValue )
	    speedValues.push_back( *iValue );
    }

    speedValues.shrink_to_fit();    
    return speedValues;
}


std::vector<double>
Trip::accelerationValues() const
{
    const_cast<Trip&>(*this).generateSegments();
    std::vector<double> accelerationValues;
    accelerationValues.reserve( m_rawData.size() - 2 );

    for ( std::vector< Segment* >::const_iterator iSegment = m_segments.begin();
	  iSegment != m_segments.end(); ++iSegment ) {
	std::vector<double> segmentValues = (*iSegment)->accelerationValues();
	for ( std::vector<double>::const_iterator iValue = segmentValues.begin();
	      iValue != segmentValues.end(); ++iValue )
	    accelerationValues.push_back( *iValue );
    }
    
    accelerationValues.shrink_to_fit();
    return accelerationValues;
}



std::vector<double>
Trip::directionValues() const
{
    const_cast<Trip&>(*this).generateSegments();
    std::vector<double> directionValues;
    directionValues.reserve( m_rawData.size() - 2 );

    for ( std::vector< Segment* >::const_iterator iSegment = m_segments.begin();
	  iSegment != m_segments.end(); ++iSegment ) {
	std::vector<double> segmentValues = (*iSegment)->angularValues();
	for ( std::vector<double>::const_iterator iValue = segmentValues.begin();
	      iValue != segmentValues.end(); ++iValue )
	    directionValues.push_back( *iValue );
    }

    directionValues.shrink_to_fit();
    return directionValues;
}



std::vector< std::tuple<double,double,double> >
Trip::speedAccelerationDirectionValues() const
{
    const_cast<Trip&>(*this).generateSegments();
    std::vector< std::tuple<double,double,double> > speedAccelerationDirectionValues;
    speedAccelerationDirectionValues.reserve( m_rawData.size() - 2 );

    for ( std::vector< Segment* >::const_iterator iSegment = m_segments.begin();
	  iSegment != m_segments.end(); ++iSegment ) {
	std::vector< std::tuple<double,double,double> > segmentValues = (*iSegment)->speedAccelerationDirectionValues();
	for ( std::vector< std::tuple<double,double,double> >::const_iterator iValue = segmentValues.begin();
	      iValue != segmentValues.end(); ++iValue )
	    speedAccelerationDirectionValues.push_back( *iValue );
    }

    return speedAccelerationDirectionValues;

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


std::vector< double >
Trip::speedQuantiles() const
{
    std::vector<double> values = this->speedValues();
    return findQuantiles( values );
}


std::vector< double >
Trip::accelerationQuantiles() const
{
    std::vector<double> values = this->accelerationValues();
    return findQuantiles( values );
}


std::vector< double >
Trip::directionQuantiles() const
{
    std::vector<double> values = this->directionValues();
    return findQuantiles( values );
}


