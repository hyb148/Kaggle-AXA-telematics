#include "Utilities.h"
#include <cmath>
#include <algorithm>

double
innerProduct( const std::pair<float,float>& v1,
            const std::pair<float,float>& v2 )
{
    return v1.first * v2.first + v1.second * v2.second;
}


double
magnitude( const std::pair<float,float>& v )
{
    return std::sqrt( innerProduct(v,v) );
}

std::pair<float,float>
operator- ( const std::pair<float,float>& v1,
           const std::pair<float,float>& v2 )
{
    return std::make_pair(v1.first - v2.first, v1.second - v2.second );
}

std::pair<float,float>
operator+ ( const std::pair<float,float>& v1,
           const std::pair<float,float>& v2 )
{
    return std::make_pair(v1.first + v2.first, v1.second + v2.second );
}

std::pair<float,float>
operator*( double f, const std::pair<float,float>& v )
{
    return std::make_pair(v.first * f, v.second * f );
}

std::pair<float,float>
operator*( const std::pair<float,float>& v, double f )
{
    return std::make_pair(v.first * f, v.second * f );
}

std::pair<float,float>
operator/( const std::pair<float,float>& v, double f )
{
    return std::make_pair(v.first / f, v.second / f );
}


std::vector< double >
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


double
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
