#ifndef DRIVERDATAPROCESSING_H
#define DRIVERDATAPROCESSING_H

#include <string>
#include <memory>
#include "Driver.h"
#include "TripMetrics.h"

class DriverDataProcessing {
public:
    // Constructor
    explicit DriverDataProcessing( const std::string& driversDirectory );
    
    // Destructor
    virtual ~DriverDataProcessing();

    // Loads all the trip data in memory
    std::vector< std::auto_ptr<Driver> > loadAllData( int numberOfThreads = 6 ) const;

    // Produces the trip metrics for all trivers and trips
    void produceTripMetrics( std::vector< TripMetrics >& outputData,
			     int numberOfThreads = 6 ) const;
    
private:
    std::string m_driversDirectory;
};

#endif
