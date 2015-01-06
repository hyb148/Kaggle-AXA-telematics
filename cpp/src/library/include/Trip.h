#ifndef TRIP_H
#define TRIP_H

#include <string>
#include <vector>
#include <utility>

class Trip {
public:
    // Constructor
    Trip();
    
    // Destructor
    virtual ~Trip();
    
    // Read raw data from csv file
    unsigned long readFromCSV( const std::string& fileName );

private:
    // The raw data
    std::vector< std::pair<float, float> > m_rawData;
};

#endif
