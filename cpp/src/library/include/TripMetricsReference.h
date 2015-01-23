#ifndef TRIPMETRICSREFERENCE_H
#define TRIPMETRICSREFERENCE_H

#include "TripMetrics.h"

#include <vector>

class Histogram;

class TripMetricsReference
{
 public:
    // Constructor
    TripMetricsReference( const std::vector< TripMetrics >& input,
                         long binsForHistograms,
                         bool trimExtremes = true );

    // Destructor
    ~TripMetricsReference();

    // Returns the probability values for a given metrics set
    std::vector<double> scoreMetrics( const TripMetrics& input ) const;
    
    // Returns the normalised standard deviations for each metric
    std::vector<double> metricStd() const;

 private:
    std::vector< Histogram* > m_histograms;
};

#endif
