#include "Trip.h"
#include <fstream>
#include <sstream>

Trip::Trip():
  m_rawData()
{
}

Trip::~Trip()
{}


unsigned long
Trip::readFromCSV( const std::string& fileName ) {
    m_rawData.clear();
    
    // Open the file
    std::ifstream inputFile( fileName );
    // Create a line buffer
    std::string lineBuffer;
    // Read the header line
    std::getline( inputFile, lineBuffer );
    
    // Read the file and store the raw data
    while ( std::getline( inputFile, lineBuffer ) ) {
        float x,y;
        char c;
        std::istringstream is( lineBuffer );
        is >> x >> c >> y;
        m_rawData.push_back( std::make_pair(x,y) );
    }
    
    // Reduce the internal capacity of the vector for memory optimisation
    m_rawData.shrink_to_fit();
    
    return m_rawData.size();
}
