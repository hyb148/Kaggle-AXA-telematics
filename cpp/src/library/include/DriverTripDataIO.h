#ifndef DRIVERTRIPDATAIO_H
#define DRIVERTRIPDATAIO_H

#include <string>
#include <vector>
#include <utility>

class DriverTripDataIO {
public:
    // Constructor
    explicit DriverTripDataIO( int driverId = 0 );
    
    // Destructor
    virtual ~DriverTripDataIO();
    
    // Returns the driver id
    inline int id() const { return m_driverId; }
    
    // Read raw data from the csv files
    DriverTripDataIO& readTripDataFromCSVFiles( const std::string& driverDirectoryName );
    
    // Write raw data to binary file
    const DriverTripDataIO& writeDataToBinaryFile( const std::string& driverDirectoryName ) const;
    
    // Reads raw data from a binary file
    DriverTripDataIO& readDataFromBinaryFile( const std::string& driverDirectoryName );
    
    inline const std::vector< std::pair< int, std::vector< std::pair<float,float> > > >& rawData() const {
        return m_rawData;
    }

private:
    // The driver id
    int m_driverId;
    
    // The raw data of a driver's trip
    std::vector< std::pair< int, std::vector< std::pair<float,float> > > > m_rawData;
};

#endif
