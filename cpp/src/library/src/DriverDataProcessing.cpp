#include "DriverDataProcessing.h"
#include "DriverTripDataIO.h"
#include "DirectoryListing.h"
#include "ProcessLogger.h"
#include "TripMetricsReference.h"

#include <thread>
#include <mutex>
#include <sstream>
#include <exception>


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


void
DriverDataProcessing::scoreTrips( std::vector< std::tuple< long, long, double > >& output,
				  int numberOfThreads ) const
{
    // First calculate the trip metrics
    TripMetricsReference masterReference;
    std::vector< TripMetrics > tripMetrics;
    size_t numberOfDrivers = this->produceTripMetrics( tripMetrics, numberOfThreads );
    masterReference.initialise( tripMetrics);

    const std::vector<double>& weights = masterReference.std();
    double sumOfWeights = 0;
    for ( std::vector<double>::const_iterator iWeight = weights.begin();
	  iWeight != weights.end(); ++iWeight ) sumOfWeights += *iWeight;

    ProcessLogger log( numberOfDrivers, "Calculating the trip scores : " );

    // For each driver construct the local reference and then score the trips within the metrics set.
    long driverId = tripMetrics.front().driverId;
    size_t startingIndex = 0;
    for ( size_t i = 1; i < tripMetrics.size(); ++i ) {
	
	long currentDriverId = tripMetrics[i].driverId;
	if ( currentDriverId == driverId && i < tripMetrics.size() - 1 )
	    continue;

	size_t lastIndex = i - 1;
	if ( i == tripMetrics.size() - 1 ) ++lastIndex;

	if ( lastIndex - startingIndex != 199 )
	    throw std::runtime_error("Wrong number of entries");

	// Select the metrics of the driver
	std::vector< TripMetrics > driverMetrics( tripMetrics.begin() + startingIndex, tripMetrics.begin() + lastIndex + 1 );


	// Create the driver reference
	TripMetricsReference* driverReference = masterReference.createUsingReference( driverMetrics );

	// Now score the trips for this driver
	for ( std::vector< TripMetrics >::const_iterator iTripMetrics = driverMetrics.begin();
	      iTripMetrics != driverMetrics.end(); ++iTripMetrics ) {

	    std::vector<double> scoresFromDriver = driverReference->scoreMetrics( *iTripMetrics );
	    std::vector<double> scoresFromReference = masterReference.scoreMetrics( *iTripMetrics );

	    // Check the sizes!
	    if (scoresFromDriver.size() != scoresFromReference.size() )
		throw std::runtime_error("Different sizes for score vectors received!");

	    if (scoresFromDriver.size() == 0 )
		throw std::runtime_error("Zero sized vectors returned!!!");

	    
	    // Calculate the average score
	    double score = 0;
	    if ( scoresFromDriver.size() == 1 ) {
		score = scoresFromDriver[0] / ( scoresFromReference[0] + scoresFromDriver[0] );
	    }
	    else {
		for ( size_t iScore = 0; iScore < scoresFromDriver.size(); ++ iScore ) {
		    double probabilityFromDriver = scoresFromDriver[iScore];
		    if ( probabilityFromDriver == 0 ) {
			std::ostringstream os;
			os << "Driver " << iTripMetrics->driverId << ", trip " << iTripMetrics->tripId << ", metric " << iScore << " : Probability from driver found 0!!!";
			throw std::runtime_error( os.str() );
		}
		    double probabilityFromReference = scoresFromReference[iScore];
		    score += weights[iScore] * probabilityFromDriver / ( probabilityFromDriver + probabilityFromReference );
		}
		score /= sumOfWeights;
	    }
	    
	    // Report the score
	    long tripId = iTripMetrics->tripId;
	    output.push_back( std::make_tuple(driverId, tripId, score ) );
	}
	
	delete driverReference;

	startingIndex = i;
	driverId = currentDriverId;
	log.taskEnded();
    }
    
}
