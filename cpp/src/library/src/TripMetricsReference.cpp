#include "TripMetricsReference.h"
#include "Histogram.h"
#include "Utilities.h"
#include "ProcessLogger.h"
#include "PCA.h"
#include <exception>
#include <sstream>
#include <cmath>
#include <algorithm>

TripMetricsReference::TripMetricsReference( const std::vector< TripMetrics >& input,
                                            long binsForHistograms ):
m_histograms(),
m_binsForHistograms( binsForHistograms ),
m_meanValues(),
m_stdValues(),
m_pca( 0 )
{
    // Create the vectors to feed the histograms
    if ( input.size() == 0 ) {
        throw std::runtime_error( "TripMetricsReference::TripMetricsReference : 0 size input given." );
    }
    
    ProcessLogger log(3, "Building the trip reference : ");
    
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
    
    log.taskEnded();
    
    // For each vector create the corresponding histogram
    for ( size_t iValue = 0; iValue < numberOfHistograms; ++iValue ) {
        // Calculate the edges to be 1% of the bin size beyond the minimum and maximum value
        double lowEdge = minValues[iValue];
        double highEdge = maxValues[iValue];
        std::vector<double>& valuesForMetric = allValues[iValue];

        // Trim extremes!
        std::sort( valuesForMetric.begin(), valuesForMetric.end() );
        const double percentageToKeep = 99.5;
        size_t lowEdgeIndex = static_cast< size_t>(std::floor( valuesForMetric.size() * (100 - percentageToKeep) / 200 ) );
        lowEdge = valuesForMetric[lowEdgeIndex];
        size_t highEdgeIndex = static_cast< size_t>(std::floor( valuesForMetric.size() * (100 + percentageToKeep) / 200 ) ) + 1;
        highEdge = valuesForMetric[highEdgeIndex];

        double binSize = ( highEdge - lowEdge ) / binsForHistograms;
        highEdge += 0.01 * binSize;
        
        // Create the histogram
        m_histograms.push_back( new Histogram( valuesForMetric,
                                               binsForHistograms,
                                               lowEdge,
                                               highEdge ) );
        
        // Trim the values (so that the sorting is not repeated at the next step
        valuesForMetric = std::vector<double>( valuesForMetric.begin()+lowEdgeIndex, valuesForMetric.begin()+highEdgeIndex );
    }
    
    log.taskEnded();
    
    // Create the PCA histograms
    allValues.clear();
    allValues.reserve( input.size() );
    for ( std::vector< TripMetrics >::const_iterator iSample = input.begin(); iSample != input.end(); ++iSample )
        allValues.push_back( iSample->values() );
    
    this->performPCA( allValues );
    
    log.taskEnded();
}





TripMetricsReference::TripMetricsReference( const std::vector< TripMetrics >& input,
                                           long binsForHistograms,
                                           const TripMetricsReference& reference ):
m_histograms(),
m_binsForHistograms( binsForHistograms ),
m_meanValues(),
m_stdValues(),
m_pca( 0 )
{
    // Create the vectors to feed the histograms
    if ( input.size() == 0 ) {
        throw std::runtime_error( "TripMetricsReference::TripMetricsReference : 0 size input given." );
    }
    
    const size_t numberOfHistograms = input.front().values().size();
    
    std::vector< std::vector<double> > allValues( numberOfHistograms, std::vector<double>() );
    
    // Loop over the trip metrics and fill in the valarrays, minimum and maximum values
    for ( size_t iMetric = 0; iMetric < input.size(); ++iMetric ) {
        
        const std::vector<double>& metricValues = input[iMetric].values();

        for ( size_t iValue = 0; iValue < numberOfHistograms; ++iValue ) {
            if ( std::isnan(metricValues[iValue])) continue;
            double currentValue = metricValues[iValue];
            allValues[iValue].push_back( currentValue );
        }
    }
    
    
    // For each vector create the corresponding histogram
    for ( size_t iValue = 0; iValue < numberOfHistograms; ++iValue ) {

        std::vector<double>& valuesForMetric = allValues[iValue];

        double lowEdge = reference.m_histograms[iValue]->lowEdge();
        double highEdge = reference.m_histograms[iValue]->highEdge();
        double binSize = ( (highEdge/1.01) - lowEdge ) / binsForHistograms;
        
        // Create the histogram
        m_histograms.push_back( new Histogram( valuesForMetric,
                                              binsForHistograms,
                                              lowEdge,
                                              highEdge ) );
        
    }

    
    
    
    // Get rid of empty values and normalise
    allValues.clear();
    allValues.reserve( input.size() );
    for ( std::vector< TripMetrics >::const_iterator iSample = input.begin(); iSample != input.end(); ++iSample )
        allValues.push_back( iSample->values() );

    const long nBinaryVariables = TripMetrics::numberOfBinaryMetrics();
    const long numberOfFeatures = numberOfHistograms - nBinaryVariables;
    
    std::vector< std::vector< double > > cleanData;
    cleanData.reserve( allValues.size() );
    
    m_meanValues = reference.m_meanValues;
    m_stdValues = reference.m_stdValues;
    for ( size_t iSample = 0; iSample < allValues.size(); ++iSample ) {
        bool nanFound = false;
        const std::vector<double>& metricValues = allValues[iSample];
        for ( size_t iMetric = 0; iMetric < metricValues.size(); ++iMetric ) {
            if ( std::isnan( metricValues[iMetric]) ) {
                nanFound = true;
                break;
            }
        }
        if ( nanFound ) continue;
        std::vector<double> sampleValues = std::vector<double>( metricValues.begin() + nBinaryVariables, metricValues.end() );
        // Normalise using previously calculated mean and std values.
        for ( size_t iFeature = 0; iFeature < numberOfFeatures; ++iFeature ) sampleValues[iFeature] = (sampleValues[iFeature] - m_meanValues[iFeature] ) / m_stdValues[iFeature];
        cleanData.push_back( sampleValues );
    }
    
    // Transform the clean data using the reference pca obejcts and create the corresponding histograms
    m_pca = new PCA( *(reference.m_pca ) );

    for ( std::vector< std::vector< double > >::iterator iData = cleanData.begin(); iData != cleanData.end(); ++iData )
        *iData = m_pca->transform( *iData );
    
    const size_t nPrincipalComponents = cleanData.front().size();
    const size_t numberOfSamples = cleanData.size();
    m_histogramsPCA.reserve( nPrincipalComponents );
    for ( size_t iComponent = 0; iComponent < nPrincipalComponents; ++iComponent ) {
        std::vector< double > histogramData( numberOfSamples, 0.0 );
        for ( size_t iSample = 0; iSample < numberOfSamples; ++iSample ) {
            histogramData[iSample] = cleanData[iSample][iComponent];
        }
        
        // Create the histogram
        m_histogramsPCA.push_back( new Histogram( histogramData,
                                                 m_binsForHistograms,
                                                 reference.m_histogramsPCA[iComponent]->lowEdge(),
                                                 reference.m_histogramsPCA[iComponent]->highEdge() ) );
    }
}



TripMetricsReference::~TripMetricsReference()
{
    for ( std::vector< Histogram* >::iterator iHistogram = m_histograms.begin();
         iHistogram != m_histograms.end(); ++iHistogram )
        delete *iHistogram;
    
    if ( m_pca) delete m_pca;
}


std::vector<double>
TripMetricsReference::scoreMetrics( const TripMetrics& input ) const
{
    size_t nSize = m_histograms.size();
    std::vector<double> result;
    result.reserve( nSize );
    
    const std::vector<double>& values = input.values();
    
    // Check it there is a nan value
    bool nanFound = false;
    for ( size_t i = 0; i < nSize; ++i ) {
        if ( std::isnan( values[i] ) ) {
            nanFound = true;
            break;
        }
    }
    
        //    if ( nanFound ) { // use the metrics histograms for scoring
    if ( true ) { // use the metrics histograms for scoring
        for ( size_t i = 0; i < nSize; ++i ) {
            const double prob = m_histograms[i]->probability( values[i] );
            result.push_back( prob );
        }
    }
    else { // use the PCA histograms for scoring
        std::vector<double> dataForPCA = std::vector<double>( values.begin() + TripMetrics::numberOfBinaryMetrics(),
                                                             values.end() );
        nSize = dataForPCA.size();
        
        // Normalise
        for ( size_t iFeature = 0; iFeature < nSize; ++iFeature )
            dataForPCA[iFeature] = (dataForPCA[iFeature] - m_meanValues[iFeature] ) / m_stdValues[iFeature];
        // Transform
        dataForPCA = m_pca->transform( dataForPCA );
        const size_t nPrincipalComponents = dataForPCA.size();
        for ( size_t iComponent = 0; iComponent < nPrincipalComponents; ++ iComponent ) {
            const double prob = m_histogramsPCA[iComponent]->probability( dataForPCA[iComponent]);
            if (std::isnan( prob ) )
                throw std::runtime_error( "TripMetricsReference::scoreMetrics : nan probability value from PCA" );
            result.push_back( prob );
        }
    }
    
    return result;
}


void
TripMetricsReference::performPCA( const std::vector< std::vector<double> >& input )
{
    const long nBinaryVariables = TripMetrics::numberOfBinaryMetrics();
    
    // Get rid of empty values.
    std::vector< std::vector< double > > cleanData;
    cleanData.reserve( input.size() );
    for ( size_t iSample = 0; iSample < input.size(); ++iSample ) {
        bool nanFound = false;
        const std::vector<double>& metricValues = input[iSample];
        for ( size_t iMetric = nBinaryVariables; iMetric < metricValues.size(); ++iMetric ) {
            if ( std::isnan( metricValues[iMetric]) ) {
                nanFound = true;
                break;
            }
            
        }
        if ( nanFound ) continue;
        cleanData.push_back( std::vector<double>( metricValues.begin() + nBinaryVariables, metricValues.end() ) );
    }
    
    // Get rid of the extreme values
    const double percentageToKeep = 99.8;
    std::vector<std::vector<double> > cleanDataNoExtremes = tripExtremesFromColumns( cleanData, percentageToKeep );
    
    
    // Then normalize the values. Store the mean and std to be used when scoring
    const size_t numberOfFeatures = cleanDataNoExtremes.front().size();
    const size_t numberOfSamples = cleanDataNoExtremes.size();
    m_meanValues = std::vector< double >( numberOfFeatures, 0.0 );
    m_stdValues = std::vector< double >( numberOfFeatures, 0.0 );
    
    for ( size_t iFeature = 0; iFeature < numberOfFeatures; ++iFeature ) {
        // calculate mean and standard deviation
        double sx = 0;
        double sxx = 0;
        for ( size_t iSample = 0; iSample < cleanDataNoExtremes.size(); ++iSample ) {
            const double x = cleanDataNoExtremes[iSample][iFeature];
            sx += x;
            sxx += x*x;
        }
        
        const double mx = sx / numberOfSamples;
        const double stdx  = std::sqrt( (sxx / numberOfSamples) - mx*mx );
        
        m_meanValues[iFeature] = mx;
        m_stdValues[iFeature] = stdx;

        // normalise values
        for ( size_t iSample = 0; iSample < cleanDataNoExtremes.size(); ++iSample ) {
            const double x = cleanData[iSample][iFeature];
            const double xnew = (x - mx) / stdx;
            cleanDataNoExtremes[iSample][iFeature] = xnew;
        }

    }
    
    if ( m_pca ) delete m_pca;
    m_pca = new PCA;
    
    // Find the principal components using the clean sample without the extremes
    m_pca->fit( cleanDataNoExtremes );
    
    
    // Transform the clean data to identify the histogram edges
    for ( std::vector< std::vector< double > >::iterator iData = cleanDataNoExtremes.begin(); iData != cleanDataNoExtremes.end(); ++iData )
        *iData = m_pca->transform( *iData );
    const size_t nPrincipalComponents = cleanDataNoExtremes.front().size();
    std::vector<double> minValues = cleanDataNoExtremes.front();
    std::vector<double> maxValues = minValues;
    for ( size_t iComponent = 0; iComponent < nPrincipalComponents; ++iComponent ) {
        for (size_t i = 0; i < cleanDataNoExtremes.size(); ++i ) {
            const double value = cleanDataNoExtremes[i][iComponent];
            if ( value < minValues[iComponent] ) minValues[iComponent] = value;
            if ( value > maxValues[iComponent] ) maxValues[iComponent] = value;
        }
    }
    
    // Normalise the full reference data
    
    
    // Normalise and tranform the full reference data
    for ( std::vector< std::vector< double > >::iterator iData = cleanData.begin(); iData != cleanData.end(); ++iData ) {
        std::vector<double>& sampleData = *iData;
        // Normalise
        for ( size_t iFeature = 0; iFeature < numberOfFeatures; ++iFeature )
            sampleData[iFeature] = (sampleData[iFeature] - m_meanValues[iFeature] ) / m_stdValues[iFeature];
        // Transform
        sampleData = m_pca->transform( sampleData );
    }
    
    // Create the histograms with the principal component values
    m_histogramsPCA.reserve( nPrincipalComponents );
    
    for ( size_t iComponent = 0; iComponent < nPrincipalComponents; ++iComponent ) {
        std::vector< double > histogramData( numberOfSamples, 0.0 );
        double minValue = minValues[iComponent];
        double maxValue = maxValues[iComponent];
        for ( size_t iSample = 0; iSample < cleanData.size(); ++iSample ) {
            const double value = cleanData[iSample][iComponent];
            histogramData[iSample] = value;
        }
        
        // Determine the edges, the bins and create the corresponding histogram.
        double binSize = ( maxValue - minValue ) / m_binsForHistograms;
        maxValue += 0.01 * binSize;
        
            // Create the histogram
        m_histogramsPCA.push_back( new Histogram( histogramData,
                                                 m_binsForHistograms,
                                                 minValue,
                                                 maxValue ) );
    }
}
