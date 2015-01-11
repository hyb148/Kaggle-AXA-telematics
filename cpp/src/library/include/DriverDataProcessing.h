#ifndef DRIVERDATAPROCESSING_H
#define DRIVERDATAPROCESSING_H

#include <string>
#include <memory>
#include "Driver.h"

class DriverDataProcessing {
public:
    // Constructor
    explicit DriverDataProcessing( const std::string& driversDirectory );
    
    // Destructor
    virtual ~DriverDataProcessing();
    
    std::vector< std::auto_ptr<Driver> > loadAllData( int numberOfThreads = 6 ) const;
    
private:
    std::string m_driversDirectory;
};

#endif
