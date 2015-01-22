#include "Histogram.h"
#include <cmath>
#include <ostream>
#include <iomanip>

Histogram::Histogram( const std::vector<double>& data,
                     long nBins,
                     double lowEdge,
                     double highEdge ):
m_prob( nBins + 2, 0. ),
m_bins( nBins ),
m_lowEdge( lowEdge ),
m_highEdge( highEdge ),
m_binSize( ( highEdge - lowEdge ) / nBins )
{
    for ( std::vector<double>::const_iterator iValue = data.begin();
         iValue != data.end(); ++iValue ) {
        
        if ( std::isnan(*iValue) ) continue;
        
        if ( *iValue < m_lowEdge ) {
            m_prob[0] += 1.0;
        }
        else if ( *iValue > m_highEdge ) {
            m_prob[ m_bins + 1 ] += 1.0;
        }
        else {
            long binId = static_cast<long>( std::floor( ( *iValue - m_lowEdge ) / m_binSize ) ) + 1;
            m_prob[binId] += 1.0;
        }
    }
    
    const double normalisationFactor = 1.0 / ( m_binSize * data.size() );
    for ( std::vector<double>::iterator iProb = m_prob.begin();
         iProb != m_prob.end(); ++iProb )
        (*iProb) *= normalisationFactor;
}



Histogram::~Histogram()
{}


double
Histogram::probability( double value ) const
{
    if ( std::isnan(value) ) return NAN;
    if ( value < m_lowEdge )
        return m_prob[0];
    if ( value > m_highEdge )
        return m_prob[ m_bins + 1 ];
    long binId = static_cast<long>( std::floor( ( value - m_lowEdge ) / m_binSize ) ) + 1;
    return m_prob[binId];
}


std::ostream&
Histogram::contents( std::ostream& os ) const
{
    os << std::fixed << std::setprecision(3)
    << "Low Edge  : " << m_lowEdge << std::endl
    << "High Edge : " << m_highEdge << std::endl
    << "Bins      : " << m_bins << std::endl
    << "Bin size  : " << m_binSize << std::endl
    << "Values    : ";
    for ( std::vector< double >::const_iterator iValue = m_prob.begin();
         iValue != m_prob.end(); ++iValue ) {
        os << *iValue << ",  ";
    }
    os << std::endl;
    return os;
}



double
Histogram::normalisedStandardDeviation() const
{
    const double range = m_highEdge - m_lowEdge;
    std::vector<double> binCentres( m_bins, 0 );
    
    for ( size_t i = 0; i < binCentres.size(); ++i ) {
        binCentres[i] = (i + 0.5) * m_binSize / range;
    }
    
    double swx = 0;
    double sw = 0;
    
    for ( size_t i = 0; i < binCentres.size(); ++i ) {
        sw += binCentres[i];
        swx += binCentres[i] * m_prob[i+1];
    }
    
    double xbar = swx / sw;
    double sd2 = 0;
    
    for ( size_t i = 0; i < binCentres.size(); ++i ) {
        sd2 += ( binCentres[i] - xbar ) * ( binCentres[i] - xbar ) * m_prob[i+1];
    }
    sd2 /= sw;
    
    return std::sqrt( sd2 );
}
