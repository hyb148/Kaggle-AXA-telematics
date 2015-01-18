#include "DriverDataProcessing.h"
#include "DriverTripDataIO.h"
#include "DirectoryListing.h"
#include "ProcessLogger.h"

#include <thread>
#include <mutex>
#include <sstream>


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
    
    ProcessLogger log( driverFiles.size() );
    
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



void
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
    
    outputData.clear();
    outputData.reserve( driverFiles.size() * 200 );
    
    std::mutex inputMutex; // Mutex for protecting the input driver list operations
    std::mutex outputMutex; // Mutex for protecting the output driver vector operations
    
    ProcessLogger log( driverFiles.size() );
    
    std::vector<std::thread> threads;
    for ( int i = 0; i < numberOfThreads; ++i ) {
        threads.push_back( std::thread( metricsThreadFunction, &inputMutex, &driverFiles, &outputMutex, &outputData, &log ) );
    }
    
    for ( int i = 0; i < numberOfThreads; ++i ) {
        threads[i].join();
    }
}
