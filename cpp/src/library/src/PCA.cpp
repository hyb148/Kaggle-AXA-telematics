#include "PCA.h"

#include <vector>
#include <algorithm>
#include <exception>

#define ARMA_NO_DEBUG
#include <armadillo>


PCA::PCA():
m_eigPairs()
{}

PCA::~PCA()
{}

PCA&
PCA::fit( const std::vector< std::vector< double > >& data )
{
    const size_t nSamples = data.size();
    const size_t nFeatures = data.front().size();
    
    // Calculate the means vector
    arma::mat meansVector( nFeatures, 1 );
    for ( size_t iFeature = 0; iFeature < nFeatures; ++iFeature ) {
        double sx = 0;
        for ( size_t iSample = 0; iSample < nSamples; ++iSample ) {
            const double value = data[iSample][iFeature];
            if ( std::isnan(value) )
                throw std::runtime_error( "PCA::fit : nan value encountered at input!" );
            sx += value;
        }
        meansVector(iFeature, 0) = sx / nSamples;
    }
    
    // Construct the covariance matrix from the scatter matrix
    arma::mat covMatrix( nFeatures, nFeatures, arma::fill::zeros );
    for ( size_t iSample = 0; iSample < nSamples; ++iSample ) {
        arma::mat sampleData( data[iSample ] );
        arma::mat d = sampleData - meansVector;
        covMatrix += d * d.t();
    }
    covMatrix /= nSamples;
    
    // Now find the eigenvalues and eigenvectors
    arma::cx_vec eigval;
    arma::cx_mat eigvec;
    bool result = arma::eig_gen(eigval, eigvec, covMatrix );
    if (! result ) {
        throw std::runtime_error("PCA::fit : eigenvalue decomposition failed!");
    }
    
    // Normalise the eigenvalues
    m_eigPairs.clear();
    m_eigPairs.reserve( nFeatures );
    double eigSum = 0;
    for ( size_t iFeature = 0; iFeature < nFeatures; ++iFeature ) {
        const double eigMagnitude = std::abs( eigval(iFeature) );

        arma::cx_vec eigenVectorFromCalculation = eigvec.row( iFeature );
        std::vector<double> eigenVector( nFeatures, 0.0 );
        for (size_t j = 0; j < nFeatures; ++j ) eigenVector[j] = eigenVectorFromCalculation(j).real();
        
        m_eigPairs.push_back( std::make_pair( eigMagnitude,
                                             eigenVector ) );
        eigSum += eigMagnitude;
    }
    
    for ( size_t iFeature = 0; iFeature < nFeatures; ++iFeature ) {
        m_eigPairs[iFeature].first /= eigSum;
    }
    
    // Sort the eigenValues
    std::sort( m_eigPairs.begin(), m_eigPairs.end(),
              [] (const std::pair<double, std::vector<double> >&a,
                  const std::pair<double, std::vector<double> >&b ) { return a.first > b.first; } );
    
    return *this;
}




std::vector< double >
PCA::transform( const std::vector< double >& data,
                double ratioOfVarianceToRetain ) const
{
    size_t nFeatures = m_eigPairs.size();
    if ( data.size() != nFeatures )
        throw std::runtime_error("PCA::transform : invalid dimension of input");
    
    std::vector< double > result;
    result.reserve( nFeatures );
    
    double varianceRetained = 0;
    size_t iFeature = 0;
    
    while ( varianceRetained < ratioOfVarianceToRetain ) {
        // perform the dot multiplication
        const std::vector<double>& eigenVector = m_eigPairs[iFeature].second;
        double x = 0;
        for (size_t i = 0; i < nFeatures; ++i ) x += eigenVector[i] * data[i];

        result.push_back( x );
        
        varianceRetained += m_eigPairs[iFeature].first;
        ++iFeature;
    }
    
    result.shrink_to_fit();

    return result;
}
