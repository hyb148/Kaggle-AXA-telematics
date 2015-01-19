#include "TripMetricsReference.h"
#include "Histogram.h"
#include "ProcessLogger.h"

#include <algorithm>
#include <exception>
#include <cmath>

TripMetricsReference::TripMetricsReference():
    m_zeroSegmentRatio( 0 ),
    m_lowPointsRatio( 0 ),
    m_histograms(),
    m_transformations( TripMetrics::transformations() ),
    m_validityChecks( TripMetrics::validityChecks() ),
    m_originalEdges(),
    m_generated( false ),
    m_std()
{}


TripMetricsReference::~TripMetricsReference()
{
    this->clear();
}


TripMetricsReference&
TripMetricsReference::clear()
{
    for ( std::vector< Histogram* >::iterator iHistogram = m_histograms.begin();
	  iHistogram != m_histograms.end(); ++iHistogram )
	delete *iHistogram;
    m_histograms.clear();
    m_zeroSegmentRatio = 0;
    m_lowPointsRatio = 0;
    m_originalEdges.clear();
    m_std.clear();
    return *this;
}



TripMetricsReference&
TripMetricsReference::initialise( const std::vector< TripMetrics >& metricsData )
{
    if ( m_histograms.size() > 0 )
	this->clear();

    const long numberOfMetricsVariables = 20;

    // Create the data vectors
    std::vector< std::vector<double> > vectorOfMetricsValues( numberOfMetricsVariables );
    for ( std::vector< std::vector<double> >::iterator iVector = vectorOfMetricsValues.begin();
	  iVector != vectorOfMetricsValues.end(); ++iVector ) {
	iVector->reserve( metricsData.size() );
    }

    // Loop over the metrics data and fill in the vectors
    for ( std::vector< TripMetrics >::const_iterator iMetrics = metricsData.begin();
	  iMetrics != metricsData.end(); ++iMetrics ) {

	if ( iMetrics->zeroSegments == 1 ) {
	    m_zeroSegmentRatio += 1;
	    continue;
	}
	if ( iMetrics->lessThan20Points == 1 ) {
	    m_lowPointsRatio += 1;
	    continue;
	}
	std::vector<double> values = iMetrics->values();

	for ( size_t i = 0; i < numberOfMetricsVariables; ++i ) {
	    vectorOfMetricsValues[i].push_back( values[i] );
	}
    }

    // Normalise the binary values
    m_zeroSegmentRatio /= metricsData.size();
    m_lowPointsRatio /= metricsData.size();

    const long numberOfBins = 200;
    const float percentageToKeep = 99.9;
    
    // Now create the histograms
    ProcessLogger log( numberOfMetricsVariables, "Transforming and histograming the reference data : " );
    m_histograms.reserve( numberOfMetricsVariables );
    m_originalEdges.reserve( numberOfMetricsVariables );
    for ( size_t i = 0; i < numberOfMetricsVariables; ++i ) {

	// The vector with the raw values for the current metric
	std::vector<double>& metricValues = vectorOfMetricsValues[i];

	// Fully sort the vector
	std::sort( metricValues.begin(), metricValues.end() );

	// apply the tranformation to the clean segment and the edges
	const std::function<bool(double)>& checkValidity = m_validityChecks[i];
	const std::function<double(double)>& metricTransformation = m_transformations[i];

	// Make sure the edges are within the valid field of definition of the transformation	
	size_t lowEdgeIndex = static_cast< size_t>(std::floor( metricValues.size() * (100 - percentageToKeep) / 200 ) );	
	while ( lowEdgeIndex < metricValues.size() - 1 ) {
	    if ( checkValidity( metricValues[lowEdgeIndex] ) )
		break;
	    else
		lowEdgeIndex++;
	}
	
	size_t highEdgeIndex = static_cast< size_t>(std::floor( metricValues.size() * (100 + percentageToKeep) / 200 ) ) + 1;
	while ( highEdgeIndex > 1 ) {
	    if ( checkValidity( metricValues[highEdgeIndex] ) )
		break;
	    else
		highEdgeIndex--;
	}

	double lowEdge = metricTransformation( metricValues[lowEdgeIndex] );
	double highEdge = metricTransformation( metricValues[highEdgeIndex] );
	double underflowValue = lowEdge - 1;
	double overflowValue = highEdge + 1;
	bool reversed = false;
	if (lowEdge > highEdge ) { // Check for reversion of edges
	    const double temp = highEdge;
	    highEdge = lowEdge;
	    lowEdge = temp;
	    underflowValue += 2; // correct the underflow value
	    overflowValue -=2; // correct the overflow value
	    reversed = true;
	}

	// Store the original edge values (needed for properly handling the transformation when scoring)
	m_originalEdges.push_back( std::make_tuple( metricValues[lowEdgeIndex],
						    metricValues[highEdgeIndex],
						    reversed ) );
	
	// Tranform the vector within the range of the edges
	for ( size_t j = lowEdgeIndex; j <= highEdgeIndex; ++j ) {
	    double oldValue = metricValues[j];
	    double newValue = metricTransformation( oldValue );
	    metricValues[j] = newValue;
	}
	// Replace the underflow values
	for ( size_t j = 0; j < lowEdgeIndex; ++j ) metricValues[j] = underflowValue;
	// Replace the overflow values
	for ( size_t j = highEdgeIndex + 1; j < metricValues.size(); ++j ) metricValues[j] = overflowValue;

	// create and fill in the histogram
	Histogram* hist = new Histogram( metricValues,
					 numberOfBins,
					 lowEdge,
					 highEdge );
	m_histograms.push_back( hist );
	m_std.push_back( hist->normalisedStandardDeviation() );
	log.taskEnded();
    }

    return *this;
}


TripMetricsReference*
TripMetricsReference::createUsingReference( const std::vector< TripMetrics >& metricsData )
{
    TripMetricsReference* newReference = new TripMetricsReference;

    const long numberOfMetricsVariables = 20;

    // Create the data vectors
    std::vector< std::vector<double> > vectorOfMetricsValues( numberOfMetricsVariables );
    for ( std::vector< std::vector<double> >::iterator iVector = vectorOfMetricsValues.begin();
	  iVector != vectorOfMetricsValues.end(); ++iVector ) {
	iVector->reserve( metricsData.size() );
    }

    // Loop over the metrics data and fill in the vectors
    for ( std::vector< TripMetrics >::const_iterator iMetrics = metricsData.begin();
	  iMetrics != metricsData.end(); ++iMetrics ) {

	if ( iMetrics->zeroSegments == 1 ) {
	    newReference->m_zeroSegmentRatio += 1;
	    continue;
	}
	if ( iMetrics->lessThan20Points == 1 ) {
	    newReference->m_lowPointsRatio += 1;
	    continue;
	}
	std::vector<double> values = iMetrics->values();

	for ( size_t i = 0; i < numberOfMetricsVariables; ++i ) {
	    vectorOfMetricsValues[i].push_back( values[i] );
	}
    }

    // Normalise the binary values
    newReference->m_zeroSegmentRatio /= metricsData.size();
    newReference->m_lowPointsRatio /= metricsData.size();

    // Fewer bins for these histograms!
    const long numberOfBins = 35;
    
    // Now create the histograms
    newReference->m_histograms.reserve( numberOfMetricsVariables );
    for ( size_t i = 0; i < numberOfMetricsVariables; ++i ) {

	// The vector with the raw values for the current metric
	std::vector<double>& metricValues = vectorOfMetricsValues[i];

	// Fully sort the vector
	std::sort( metricValues.begin(), metricValues.end() );

	// apply the tranformation to the clean segment and the edges
	const std::function<bool(double)>& checkValidity = m_validityChecks[i];
	const std::function<double(double)>& metricTransformation = m_transformations[i];

	// Make sure the edges are within the valid field of definition of the transformation	
	size_t lowEdgeIndex = 0;
	while ( lowEdgeIndex < metricValues.size() - 1 ) {
	    if ( checkValidity( metricValues[lowEdgeIndex] ) )
		break;
	    else
		lowEdgeIndex++;
	}
	
	size_t highEdgeIndex = metricValues.size() - 1;
	while ( highEdgeIndex > 1 ) {
	    if ( checkValidity( metricValues[highEdgeIndex] ) )
		break;
	    else
		highEdgeIndex--;
	}

	// The low edge is from the reference histogram
	double lowEdge = m_histograms[i]->lowEdge();
	double highEdge = m_histograms[i]->highEdge();
	double underflowValue = lowEdge - 1;
	double overflowValue = highEdge + 1;
	if ( metricTransformation( metricValues[highEdgeIndex] ) < metricTransformation( metricValues[lowEdgeIndex] ) ) { // reverse the overflow values
	    const double temp = underflowValue;
	    underflowValue = overflowValue;
	    overflowValue = temp;
	}

	// Tranform the vector within the range of the edges
	for ( size_t j = lowEdgeIndex; j <= highEdgeIndex; ++j ) metricValues[j] = metricTransformation( metricValues[j] );
	// Replace the underflow values
	for ( size_t j = 0; j < lowEdgeIndex; ++j ) metricValues[j] = underflowValue;
	// Replace the overflow values
	for ( size_t j = highEdgeIndex + 1; j < metricValues.size(); ++j ) metricValues[j] = overflowValue;

	// create and fill in the histogram
	Histogram* hist = new Histogram( metricValues,
					 numberOfBins,
					 lowEdge,
					 highEdge );
	
	newReference->m_histograms.push_back( hist );
    }
    
    newReference->m_originalEdges = m_originalEdges;
    newReference->m_generated = true;

    return newReference;
}


std::vector<double>
TripMetricsReference::scoreMetrics( const TripMetrics& metrics ) const
{
    std::vector<double> scores;
    
    if ( metrics.zeroSegments == 1 ) {
	scores.push_back( m_zeroSegmentRatio );
	return scores;
    }
    
    if  ( metrics.lessThan20Points == 1 ) {
	scores.push_back( m_lowPointsRatio );
	return scores;
    }
    
    scores.reserve( m_histograms.size() );
    std::vector<double> values = metrics.values();
    for ( size_t i = 0; i < m_histograms.size(); ++i ) {
	double value = values[i];
	
	// Need to transform the value (after we have checked that this is not beyond the edges
	if ( m_validityChecks[i]( value ) )
	    value = m_transformations[i]( value );
	else {	    
	    std::tuple<double,double,bool> edges = m_originalEdges[i];
	    bool reverse = std::get<2>( edges );
	    if ( ! reverse ) {
		if ( value < std::get<0>(edges) ) value = m_histograms[i]->lowEdge() - 1; // Bring it to the undeflow
		else if ( value > std::get<1>(edges) ) value = m_histograms[i]->highEdge() + 1; // Bring it to the undeflow
		else throw std::runtime_error("Unhandled case when scoring!");
	    }
	    else {
		if ( value < std::get<0>(edges) ) value = m_histograms[i]->highEdge() + 1; // Bring it to the undeflow
		else if ( value > std::get<1>(edges) ) value = m_histograms[i]->lowEdge() - 1; // Bring it to the undeflow
		else throw std::runtime_error("Unhandled case when scoring!");
	    }	    
	}
	
	double score = m_histograms[i]->probability( value );

	if ( score == 0 && m_generated ) {
	    throw std::runtime_error( "Found 0 score in generated driver metrics reference!" );
	}
	
	scores.push_back( score );
    }

    return scores;
}
