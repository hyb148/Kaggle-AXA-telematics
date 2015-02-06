#ifndef TRIPMETRICSREFERENCE_H
#define TRIPMETRICSREFERENCE_H

#include "TripMetrics.h"

#include <vector>

class Histogram;
class PCA;

class TripMetricsReference
{
public:
    // Constructor
    TripMetricsReference( const std::vector< TripMetrics >& input,
                         long binsForHistograms );
    
    // Constructor for driver data
    TripMetricsReference( const std::vector< TripMetrics >& input,
                         long binsForHistograms,
                         const TripMetricsReference& reference );

    // Destructor
    ~TripMetricsReference();

    // Returns the probability values for a given metrics set
    std::vector<double> scoreMetrics( const TripMetrics& input ) const;
    
private: // Members
    
    // The histograms of the metrics
    std::vector< Histogram* > m_histograms;
    
    // number of bins for the histograms
    long m_binsForHistograms;
    
    // The mean and std values of the variable metrics
    std::vector< double > m_meanValues;
    std::vector< double > m_stdValues;
    
    // The PCA object
    PCA* m_pca;
    
    // The histograms of the PCA components
    std::vector< Histogram* > m_histogramsPCA;

private:
    // Performs a PCA to the metrics
    void performPCA( const std::vector< std::vector<double> >& input );    
};

#endif
