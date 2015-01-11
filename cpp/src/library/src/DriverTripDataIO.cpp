#include "DriverTripDataIO.h"
#include "DirectoryListing.h"

#include <fstream>
#include <sstream>

#include <exception>

DriverTripDataIO::DriverTripDataIO( int driverId ):
  m_driverId( driverId ),
  m_rawData()
{}

DriverTripDataIO::~DriverTripDataIO()
{}


DriverTripDataIO&
DriverTripDataIO::readTripDataFromCSVFiles( const std::string& driverDirectoryName )
{
    m_rawData.clear();
    m_rawData.reserve(200);
    
    std::ostringstream osDriverDirectoryName;
    osDriverDirectoryName << driverDirectoryName << "/" << m_driverId;
    
    DirectoryListing dirListing( osDriverDirectoryName.str() );
    std::list<std::string> tripFileNames = dirListing.directoryContent();
    for (std::list<std::string>::const_iterator iTripFileName = tripFileNames.begin();
         iTripFileName != tripFileNames.end(); ++iTripFileName ) {
        std::vector< std::pair< float, float > > tripData;
        tripData.reserve(2000);
        int tripId = 0;
        std::istringstream isTripId( *iTripFileName );
        isTripId >> tripId;
        
        std::ostringstream osTripFileName;
        osTripFileName << driverDirectoryName << "/" << m_driverId << "/" << tripId << ".csv";
        
        // Open the file
        std::ifstream inputFile( osTripFileName.str() );
        // Create a line buffer
        std::string lineBuffer;
        std::getline( inputFile, lineBuffer );
        
        // Read the file and store the raw data
        while ( std::getline( inputFile, lineBuffer ) ) {
            float x,y;
            char c;
            std::istringstream is( lineBuffer );
            is >> x >> c >> y;
            tripData.push_back( std::make_pair(x,y) );
        }
        
        tripData.shrink_to_fit();
        m_rawData.push_back( std::make_pair( tripId, tripData ) );
    }
    return *this;
}

const DriverTripDataIO&
DriverTripDataIO::writeDataToBinaryFile( const std::string& driverDirectoryName ) const
{
    // Open the output file
    std::ostringstream osFileName;
    osFileName << driverDirectoryName << "/" << m_driverId << ".data";
    
    std::ofstream outputFile;
    outputFile.open( osFileName.str(), std::ios::out | std::ios::binary);
    
    if (! outputFile.is_open() )
        throw std::runtime_error( "Could not open output file ");
    
    // Write driver id and number of trips
    outputFile.write( (const char*) &m_driverId, sizeof(m_driverId) );
    unsigned long numberOfTrips = m_rawData.size();
    outputFile.write( (const char*) &numberOfTrips, sizeof(numberOfTrips) );
    
    // Loop over the trips and write the trip id, the number of data points and the data points
    for ( unsigned long i = 0; i < numberOfTrips; ++ i ) {
        int tripId = m_rawData[i].first;
        const std::vector< std::pair<float,float> >& tripData = m_rawData[i].second;
        outputFile.write( (const char*) &tripId, sizeof(tripId) );
        unsigned long numberOfPoints = tripData.size();
        outputFile.write( (const char*) &numberOfPoints, sizeof(numberOfPoints) );
        for ( unsigned long j = 0; j < numberOfPoints; ++j ) {
            outputFile.write( (const char*) &tripData[j].first, sizeof(tripData[j].first) );
            outputFile.write( (const char*) &tripData[j].second, sizeof(tripData[j].second) );
        }
    }
    outputFile.flush();
    outputFile.close();
    
    return *this;
}


DriverTripDataIO&
DriverTripDataIO::readDataFromBinaryFile( const std::string& driverDirectoryName )
{
    m_rawData.clear();

    // Open the input file
    std::ostringstream osFileName;
    osFileName << driverDirectoryName << "/" << m_driverId << ".data";
    
    std::ifstream inputFile;
    inputFile.open( osFileName.str(), std::ios::in | std::ios::binary);
    
    if (! inputFile.is_open() )
        throw std::runtime_error( "Could not open input file ");

    // Read driver id and number of trips
    inputFile.read( (char*) &m_driverId, sizeof(m_driverId) );
    unsigned long numberOfTrips = 0;
    inputFile.read( (char*) &numberOfTrips, sizeof(numberOfTrips) );
    m_rawData.reserve( numberOfTrips );
    
    // Loop over the trips and write the trip id, the number of data points and the data points
    for ( unsigned long i = 0; i < numberOfTrips; ++ i ) {
        int tripId = 0;
        std::vector< std::pair<float,float> > tripData;
        inputFile.read( ( char*) &tripId, sizeof(tripId) );
        unsigned long numberOfPoints = 0;
        inputFile.read( (char*) &numberOfPoints, sizeof(numberOfPoints) );
        tripData.reserve( numberOfPoints );
        for ( unsigned long j = 0; j < numberOfPoints; ++j ) {
            float x, y;
            inputFile.read( (char*) &x, sizeof(x) );
            inputFile.read( (char*) &y, sizeof(y) );
            tripData.push_back( std::make_pair(x,y) );
        }
        m_rawData.push_back(std::make_pair( tripId, tripData ) );
    }
    
    return *this;
}
