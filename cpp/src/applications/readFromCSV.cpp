#include <iostream>
#include <sstream>
#include <exception>

#include "DriverTripDataIO.h"
#include "DirectoryListing.h"
#include "ProcessLogger.h"

int main( int, char**) {
    try {
        std::string driverCSVDir = "drivers";
        std::string driverCompressedDir = "drivers_compressed_data";
        
        DirectoryListing dirList( driverCSVDir );
        std::list<std::string> driverDirs = dirList.directoryContent();
        
        ProcessLogger log( driverDirs.size() );
        
        for ( std::list<std::string>::const_iterator iDriverDir = driverDirs.begin();
             iDriverDir != driverDirs.end(); ++iDriverDir ) {
            
            int driverId = 0;
            std::istringstream isId( *iDriverDir );
            isId >> driverId;
            
            DriverTripDataIO dataIO( driverId );
            dataIO.readTripDataFromCSVFiles( driverCSVDir ).writeDataToBinaryFile( driverCompressedDir );
            
            log.taskEnded();
        }
        
    }
    catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
        return -1;
    }
    catch (...) {
        std::cerr << "Unknown error" << std::endl;
        return -1;
    }
    return 0;
}
