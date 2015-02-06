#include "DriverDataProcessing.h"
#include "DriverTripDataIO.h"
#include "DirectoryListing.h"
#include "ProcessLogger.h"
#include "TripMetricsReference.h"

#include <thread>
#include <mutex>
#include <sstream>
#include <exception>
#include <cmath>

DriverDataProcessing::DriverDataProcessing( const std::string& driversDirectory ):
m_driversDirectory( driversDirectory )
{}


DriverDataProcessing::~DriverDataProcessing()
{}


static void readThreadFunction( std::mutex* pinputMutex,
                               std::list<std::string>* pdriverFiles,
                               std::mutex* poutputMutex,
                               std::vector< Driver* >* pdrivers,
                               ProcessLogger* plog )
{
    std::mutex& inputMutex = *pinputMutex;
    std::list<std::string>& driverFiles = *pdriverFiles;
    std::mutex& outputMutex = *poutputMutex;
    std::vector< Driver* >& drivers = *pdrivers;
    ProcessLogger& log = *plog;
    
    while (true) {
        inputMutex.lock();
        if ( driverFiles.empty() ) {
            inputMutex.unlock();
            break;
        }
        std::string driverFile = driverFiles.front();
        driverFiles.pop_front();
        inputMutex.unlock();
        
        size_t pos = driverFile.find( "/" );
        
        int driverId = 0;
        std::istringstream isId( driverFile.substr(pos+1) );
        isId >> driverId;
        
        DriverTripDataIO driverTripDataIO( driverId );
        driverTripDataIO.readDataFromBinaryFile( driverFile.substr(0,pos) );
        
        Driver* driver = new Driver( driverTripDataIO.id() );
        driver->loadTripData( driverTripDataIO.rawData() );
        
        outputMutex.lock();
        drivers.push_back( driver );
        outputMutex.unlock();
        
        log.taskEnded();
    }
}


std::vector< std::auto_ptr<Driver> >
DriverDataProcessing::loadAllData( int numberOfThreads ) const
{
        // The driver vector
    std::vector< Driver* > drivers;
    DirectoryListing dirList( m_driversDirectory );
    std::list<std::string> driverFiles = dirList.directoryContent();
    for (std::list<std::string>::iterator iDriverFile = driverFiles.begin();
         iDriverFile != driverFiles.end(); ++iDriverFile ) {
        *iDriverFile = m_driversDirectory + "/" + *iDriverFile;
    }
    
    drivers.reserve( driverFiles.size() );
    
    std::mutex inputMutex; // Mutex for protecting the input driver list operations
    std::mutex outputMutex; // Mutex for protecting the output driver vector operations
    
    ProcessLogger log( driverFiles.size(), "Loading all trips from all drivers : " );
    
    std::vector<std::thread> threads;
    for ( int i = 0; i < numberOfThreads; ++i ) {
        threads.push_back( std::thread( readThreadFunction, &inputMutex, &driverFiles, &outputMutex, &drivers, &log ) );
    }
    
    for ( int i = 0; i < numberOfThreads; ++i ) {
        threads[i].join();
    }
    
    std::vector< std::auto_ptr<Driver> > result;
    result.reserve( drivers.size() );
    
    for ( std::vector< Driver* >::iterator iDriverPtr = drivers.begin();
         iDriverPtr != drivers.end(); ++iDriverPtr ) {
        result.push_back( std::auto_ptr<Driver>(*iDriverPtr) );
    }
    
    return result;
}





//******************************************************************************



static
void metricsThreadFunction( std::mutex* pinputMutex,
                           std::list<std::string>* pdriverFiles,
                           std::mutex* poutputMutex,
                           std::vector< TripMetrics >* pmetrics,
                           ProcessLogger* plog )
{
    std::mutex& inputMutex = *pinputMutex;
    std::list<std::string>& driverFiles = *pdriverFiles;
    std::mutex& outputMutex = *poutputMutex;
    std::vector< TripMetrics >& metrics = *pmetrics;
    ProcessLogger& log = *plog;
    
    while (true) {
        inputMutex.lock();
        if ( driverFiles.empty() ) {
            inputMutex.unlock();
            break;
        }
        std::string driverFile = driverFiles.front();
        driverFiles.pop_front();
        inputMutex.unlock();
        
        size_t pos = driverFile.find( "/" );
        
        int driverId = 0;
        std::istringstream isId( driverFile.substr(pos+1) );
        isId >> driverId;
        
        DriverTripDataIO driverTripDataIO( driverId );
        driverTripDataIO.readDataFromBinaryFile( driverFile.substr(0,pos) );
        
        Driver driver( driverTripDataIO.id() );
        driver.loadTripData( driverTripDataIO.rawData() );
        const std::vector< Trip >& trips = driver.trips();
        
        std::vector< TripMetrics > localMetrics = driver.tripMetrics();
        
        outputMutex.lock();
        for ( std::vector< TripMetrics >::const_iterator iMetrics = localMetrics.begin();
             iMetrics != localMetrics.end(); ++iMetrics )
            metrics.push_back( *iMetrics );
        outputMutex.unlock();
        log.taskEnded();
    }
}



size_t
DriverDataProcessing::produceTripMetrics( std::vector< TripMetrics >& outputData,
                                         int numberOfThreads ) const
{
        // The driver vector
    DirectoryListing dirList( m_driversDirectory );
    std::list<std::string> driverFiles = dirList.directoryContent();
    for (std::list<std::string>::iterator iDriverFile = driverFiles.begin();
         iDriverFile != driverFiles.end(); ++iDriverFile ) {
        *iDriverFile = m_driversDirectory + "/" + *iDriverFile;
    }
    
    size_t numberOfDrivers = driverFiles.size();
    
    outputData.clear();
    outputData.reserve( numberOfDrivers * 200 );
    
    std::mutex inputMutex; // Mutex for protecting the input driver list operations
    std::mutex outputMutex; // Mutex for protecting the output driver vector operations
    
    ProcessLogger log( numberOfDrivers, "Producing trip metrics from all drivers : " );
    
    std::vector<std::thread> threads;
    for ( int i = 0; i < numberOfThreads; ++i ) {
        threads.push_back( std::thread( metricsThreadFunction, &inputMutex, &driverFiles, &outputMutex, &outputData, &log ) );
    }
    
    for ( int i = 0; i < numberOfThreads; ++i ) {
        threads[i].join();
    }
    
    return numberOfDrivers;
}




//************************************** TRIP SCORING ************************************************************

void
DriverDataProcessing::scoreTrips( std::vector< std::tuple< long, long, double > >& output,
                                 int numberOfThreads ) const
{
    const long numberOfBinsBackground = 200;
    const long numberOfBinsDriver = 25;
    
    const double backgroundProportion = 0.3;
    const double signalProportion = 1.0 - backgroundProportion;
    
    // First calculate the trip metrics
    std::vector< TripMetrics > tripMetrics;
    size_t numberOfDrivers = this->produceTripMetrics( tripMetrics, numberOfThreads );
    TripMetricsReference masterReference( tripMetrics, numberOfBinsBackground );
    
    ProcessLogger log( numberOfDrivers, "Calculating the trip scores from metrics : " );
    
    // For each driver construct the local reference and then score the trips within the metrics set.
    long driverId = tripMetrics.front().driverId();
    size_t startingIndex = 0;
    for ( size_t i = 1; i < tripMetrics.size(); ++i ) {
        
        long currentDriverId = tripMetrics[i].driverId();
        if ( currentDriverId == driverId && i < tripMetrics.size() - 1 )
            continue;
        
        size_t lastIndex = i - 1;
        if ( i == tripMetrics.size() - 1 ) ++lastIndex;
        
        // Select the metrics of the driver
        std::vector< TripMetrics > driverMetrics( tripMetrics.begin() + startingIndex, tripMetrics.begin() + lastIndex + 1 );
        
        // Create the driver reference
        TripMetricsReference driverReference ( driverMetrics, numberOfBinsDriver, masterReference );
        
        // Score the trips
        for ( std::vector< TripMetrics >::const_iterator iTripMetrics = driverMetrics.begin();
             iTripMetrics != driverMetrics.end(); ++iTripMetrics ) {
            
            std::vector<double> scoreFromAll = masterReference.scoreMetrics( *iTripMetrics );
            std::vector<double> scoreFromDriver = driverReference.scoreMetrics( *iTripMetrics );
            
            if ( scoreFromDriver.size() != scoreFromAll.size() )
                throw std::runtime_error( "DriverDataProcessing::scoreTrips : unequal sizes for reference and driver" );
            
            double score = 0;
            double totalWeight = 0;
            
            for ( size_t iMetric = 0; iMetric < scoreFromAll.size(); ++iMetric ) {
                if ( std::isnan( scoreFromAll[iMetric] ) || std::isnan(scoreFromDriver[iMetric]) ) continue;
                
                double probability = signalProportion * scoreFromDriver[iMetric] / ( backgroundProportion * scoreFromAll[iMetric] + signalProportion * scoreFromDriver[iMetric] );
                if ( std::isnan( probability ) ) {
                    std::ostringstream os;
                    os << "DriverDataProcessing::scoreTrips : nan probability calculated!" << std::endl;
                    os << "   driver id       : " << driverId << std::endl;
                    os << "   trip id         : " << iTripMetrics->tripId() << std::endl;
                    os << "   iMetric         : " << iMetric << std::endl;
                    os << "   scoreFromDriver : " << scoreFromDriver[iMetric] << std::endl;
                    os << "   scoreFromAll    : " << scoreFromAll[iMetric] << std::endl;
                    throw std::runtime_error( os.str() );
                }
                score += probability;
                totalWeight += 1;
            }
            
            if (totalWeight == 0)
                throw std::runtime_error("DriverDataProcessing::scoreTrips : total weight for scoring is 0!");
            
            score /= totalWeight;
            
            long tripId = iTripMetrics->tripId();
            
            output.push_back( std::make_tuple(driverId, tripId, score ) );
        }
        
        startingIndex = i;
        driverId = currentDriverId;
        log.taskEnded();
    }

}
