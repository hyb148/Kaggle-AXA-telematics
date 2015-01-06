#include <iostream>
#include <exception>

#include "Trip.h"
#include "DirectoryListing.h"

int main( int, char**) {
    try {
        std::cout << "Starting ... " << std::endl;
        
        std::vector< Trip* > trips;
        
        DirectoryListing listing;
        listing.setWorkingDirectory("drivers");
        std::list<std::string> drivers = listing.directoryContent();
        
        // Looping over the drivers
        for (std::list<std::string>::const_iterator i = drivers.begin();
             i != drivers.end(); ++i ) {
            
            listing.setWorkingDirectory( "drivers/" + *i );
            std::list<std::string> tripFiles = listing.directoryContent();
            
            // Looping over the trip files
            for (std::list<std::string>::const_iterator t = tripFiles.begin();
                 t != tripFiles.end(); ++t ) {
                
                Trip* trip = new Trip();
                unsigned long nPoints = trip->readFromCSV( "drivers/" + *i + "/" + *t );
                
                trips.push_back( trip );
            }
            
        }

        std::cout << trips.size() << " trips loaded!" << std::endl;

        for (std::vector<Trip*>::iterator iTrip = trips.begin(); iTrip != trips.end(); ++iTrip )
            delete *iTrip;
        
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
