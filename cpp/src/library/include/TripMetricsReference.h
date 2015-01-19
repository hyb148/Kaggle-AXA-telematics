#ifndef TRIPMETRICSREFERENCE_H
#define TRIPMETRICSREFERENCE_H

#include <vector>
#include <tuple>
#include "TripMetrics.h"

class Histogram;

class TripMetricsReference
{
 public:
    TripMetricsReference();

    ~TripMetricsReference();

    // Scores a metrics object and returns the array of the individual scores
    std::vector<double> scoreMetrics( const TripMetrics& metrics ) const;

    // Initialises the internal structures from a population of metrics objects
    TripMetricsReference& initialise( const std::vector< TripMetrics >& metricsData );

    // Creates a new TripMetricsReference object using the existing internal structures for normalisation
    TripMetricsReference* createUsingReference( const std::vector< TripMetrics >& metricsData );

 private:
    // Method for clearing the internal structures
    TripMetricsReference& clear();

 private:
    // Zero Segment ratio
    double m_zeroSegmentRatio;

    // Low Points ratio
    double m_lowPointsRatio;
    
    // The internal histograms
    std::vector< Histogram* > m_histograms;

    // The transformation objects
    std::vector< std::function<double(double)> >   m_transformations;
    std::vector< std::function<bool(double)> >     m_validityChecks;

    // The edges before transformation and a flag if they have been reversed after the transformation
    std::vector< std::tuple<double,double,bool> >  m_originalEdges;

    // Flag indicating if this is a generated object
    bool m_generated;
};

#endif
