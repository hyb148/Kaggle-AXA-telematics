#ifndef HISTOGRAM_H
#define HISTOGRAM_H

#include <vector>
#include <iosfwd>

// Simple histogram class
class Histogram
{
 public:
    // Constructor
    Histogram( const std::vector< double >& data,
	       long nBins,
	       double lowEdge,
	       double highEdge );

    // Destructor
    ~Histogram();

    // Receives the probability value given an input value
    double probability( double value ) const;

    // Returns the low edge of the histogram
    inline double lowEdge() const { return m_lowEdge; }

    // Returns the high edge of the histogram
    inline double highEdge() const { return m_highEdge; }

    // Dumps the histograms to output
    std::ostream& contents( std::ostream& os ) const;

 private:
    // The histogram probability values
    std::vector< double > m_prob;

    // The histogram bins
    const long m_bins;

    // The bin size
    const double m_lowEdge;

    // The histogram origin
    const double m_highEdge;

    // The bin size
    const double m_binSize;
};


#endif
