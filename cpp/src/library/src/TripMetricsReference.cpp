#include "TripMetricsReference.h"
#include "Histogram.h"
#include "Utilities.h"
#include <exception>
#include <sstream>
#include <cmath>
#include <algorithm>

TripMetricsReference::TripMetricsReference( const std::vector< TripMetrics >& input,
                                            long binsForHistograms,
                                            bool trimExtremes ):
m_histograms()
{
    // Create the vectors to feed the histograms
    if ( input.size() == 0 ) {
        throw std::runtime_error( "TripMetricsReference::TripMetricsReference : 0 size input given." );
    }
    
    const size_t numberOfHistograms = input.front().values().size();
    
    std::vector<double> minValues = input.front().values();
    std::vector<double> maxValues = input.front().values();
    
    std::vector< std::vector<double> > allValues( numberOfHistograms, std::vector<double>() );
    
    // Loop over the trip metrics and fill in the valarrays, minimum and maximum values
    for ( size_t iMetric = 0; iMetric < input.size(); ++iMetric ) {
        
        const std::vector<double>& metricValues = input[iMetric].values();
        
        for ( size_t iValue = 0; iValue < numberOfHistograms; ++iValue ) {
            if ( std::isnan(metricValues[iValue])) continue;
            double currentValue = metricValues[iValue];
            allValues[iValue].push_back( currentValue );
            if ( std::isnan(minValues[iValue]) || currentValue < minValues[iValue] ) minValues[iValue] = currentValue;
            if ( std::isnan(maxValues[iValue]) || currentValue > maxValues[iValue] ) maxValues[iValue] = currentValue;
        }
    }
    
    // For each vector create the corresponding histogram
    for ( size_t iValue = 0; iValue < numberOfHistograms; ++iValue ) {
        // Calculate the edges to be 1% of the bin size beyond the minimum and maximum value
        double lowEdge = minValues[iValue];
        double highEdge = maxValues[iValue];
        std::vector<double>& valuesForMetric = allValues[iValue];

        if ( trimExtremes ) {
            std::sort( valuesForMetric.begin(), valuesForMetric.end() );
            const double percentageToKeep = 99.5;
            size_t lowEdgeIndex = static_cast< size_t>(std::floor( valuesForMetric.size() * (100 - percentageToKeep) / 200 ) );
            lowEdge = valuesForMetric[lowEdgeIndex];
            size_t highEdgeIndex = static_cast< size_t>(std::floor( valuesForMetric.size() * (100 + percentageToKeep) / 200 ) ) + 1;
            highEdge = valuesForMetric[highEdgeIndex];
        }

        double binSize = ( highEdge - lowEdge ) / binsForHistograms;
        highEdge += 0.01 * binSize;
        
        // Create the histogram
        m_histograms.push_back( new Histogram( valuesForMetric,
                                               binsForHistograms,
                                               lowEdge,
                                               highEdge ) );
        
    }
}


TripMetricsReference::~TripMetricsReference()
{
    for ( std::vector< Histogram* >::iterator iHistogram = m_histograms.begin();
         iHistogram != m_histograms.end(); ++iHistogram )
        delete *iHistogram;
}


std::vector<double>
TripMetricsReference::scoreMetrics( const TripMetrics& input ) const
{
    const size_t nSize = m_histograms.size();
    std::vector<double> result( nSize );
    const std::vector<double>& values = input.values();
    
    if ( values.size() != nSize ) {
        std::ostringstream os;
        os << "TripMetricsReference::scoreMetrics : TripMetrics for driver " << input.driverId()
        << ", trip " << input.tripId() << " has a size of " << values.size() << " instead of " << nSize << ".";
        throw std::runtime_error( os.str() );
    }
    
    for ( size_t i = 0; i < nSize; ++i )
        result[i] = m_histograms[i]->probability( values[i] );
    
    return result;
}
