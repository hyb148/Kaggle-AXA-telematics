#include "Segment.h"
#include <cmath>

Segment::Segment( const std::vector< std::pair<float, float> >& segmentCoordinates ):
  m_origin( segmentCoordinates.front() ),
  m_velocityVectors()
{
    size_t numberOfPoints = segmentCoordinates.size();
    m_velocityVectors.reserve( numberOfPoints - 1 );
    for (size_t i = 1; i < numberOfPoints; ++i ) {
        const std::pair<float,float>& v2 = segmentCoordinates[i];
        const std::pair<float,float>& v1 = segmentCoordinates[i-1];
        m_velocityVectors.push_back( std::make_pair(v2.first - v1.first, v2.second - v1.second ) );
    }
}
    
Segment::~Segment()
{}



std::vector< std::pair<float,float> >
Segment::dataPoints() const
{
    std::vector< std::pair<float,float> > dataPoints;
    dataPoints.reserve( m_velocityVectors.size() + 1 );
    
    dataPoints.push_back(m_origin);
    for ( std::vector< std::pair<float,float> >::const_iterator iVector = m_velocityVectors.begin();
         iVector != m_velocityVectors.end(); ++iVector ) {
        const std::pair<float,float>& previous = dataPoints.back();
        dataPoints.push_back(std::make_pair( previous.first + iVector->first, previous.second + iVector->second ));
    }
    return dataPoints;
}


std::vector<double>
Segment::speedValues() const
{
    std::vector<double> speedValues;
    speedValues.reserve(m_velocityVectors.size() );
    for (std::vector< std::pair<float,float> >::const_iterator i = m_velocityVectors.begin();
         i != m_velocityVectors.end(); ++i ) {
        const std::pair<float,float>& v = *i;
        speedValues.push_back( std::sqrt( std::pow(v.first,2) + std::pow(v.second, 2) ) );
    }
    
    return speedValues;
}

std::vector<double>
Segment::accelerationValues() const
{
    std::vector<double> accelerationValues;
    if ( m_velocityVectors.size() < 2 ) return accelerationValues;
    accelerationValues.reserve( m_velocityVectors.size() - 1 );

    const std::pair<float,float>& v = m_velocityVectors.front();
    double v_previous = std::sqrt( std::pow(v.first,2) + std::pow(v.second, 2) );

    for (std::vector< std::pair<float,float> >::const_iterator i = m_velocityVectors.begin() + 1;
         i != m_velocityVectors.end(); ++i ) {
        const std::pair<float,float>& v = *i;
        const double v_current = std::sqrt( std::pow(v.first,2) + std::pow(v.second, 2) );
	accelerationValues.push_back( v_current - v_previous );
	v_previous = v_current;
    }

    return accelerationValues;
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



std::vector< std::tuple<double,double,double> >
Segment::speedAccelerationDirectionValues() const
{
    std::vector< std::tuple<double,double,double> > sadValues;
    if ( m_velocityVectors.size() < 2 ) return sadValues;
    sadValues.reserve( m_velocityVectors.size() - 1 );

    const std::pair<float,float>* v_p = &( m_velocityVectors.front());
    double v_previous = std::sqrt( std::pow(v_p->first,2) + std::pow(v_p->second, 2) );

    for (std::vector< std::pair<float,float> >::const_iterator i = m_velocityVectors.begin() + 1;
         i != m_velocityVectors.end(); ++i ) {
        const std::pair<float,float>& v = *i;
        const double v_current = std::sqrt( std::pow(v.first,2) + std::pow(v.second, 2) );
	const double acceleration = v_current - v_previous;
	const double angle = angleAmongVectors( v, *v_p);

	sadValues.push_back( std::make_tuple( v_current, acceleration, angle ) );
	
	v_previous = v_current;
	v_p = &v;
    }

    return sadValues;
}


std::vector<double>
Segment::angularValues() const
{
    const double pi = std::atan( 1.0 ) * 4;
    
    size_t numberOfVectors = m_velocityVectors.size();
    std::vector<double> angularValues( numberOfVectors - 1, 0 );
    for (size_t i = 1; i < numberOfVectors; ++i ) {
        const std::pair<float,float>& v2 = m_velocityVectors[i];
        const std::pair<float,float>& v1 = m_velocityVectors[i-1];
	angularValues.push_back( angleAmongVectors(v2,v1) );
    }
    
    return angularValues;
}

double
Segment::travelLength() const
{
    double distance = 0;
    std::vector<double> speedValues = this->speedValues();
    for ( std::vector<double>::const_iterator i = speedValues.begin();
         i != speedValues.end(); ++i ) distance += *i;
    
    return distance;
}


long
Segment::travelDuration() const
{
    return m_velocityVectors.size() + 1;
}
