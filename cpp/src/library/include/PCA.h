#ifndef PCA_H
#define PCA_H

#include <vector>
#include <utility>

// Simple PCA
class PCA
{
 public:
    // Constructor
    PCA();

    // Destructor
    ~PCA();
    
    // Performs a PCA for a given data set
    PCA& fit( const std::vector< std::vector< double > >& data );
    
    // Transforms a vector to the principal component space
    std::vector< double > transform( const std::vector< double >& data,
                                    double ratioOfVarianceToRetain = 0.95 ) const;

 private:
    // The eigenvalues and eigenvectors
    std::vector< std::pair< double, std::vector<double> > > m_eigPairs;
    
};


#endif
