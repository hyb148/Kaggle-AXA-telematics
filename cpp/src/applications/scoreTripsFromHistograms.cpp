#include "DriverDataProcessing.h"

#include <iostream>
#include <fstream>
#include <exception>


int main( int, char** )
{
    try {
        std::string driverCompressedDir = "drivers_compressed_data";
        DriverDataProcessing dataProcessing( driverCompressedDir );

	std::vector< std::tuple< long, long, double > > output;
	dataProcessing.scoreTrips( output );

	// Now write the output file
	std::cout << "Writing the results to the file." << std::endl;
	std::ofstream outputFile;
	outputFile.open( "submission.csv", std::ios::out );
	outputFile << "driver_trip,prob" << std::endl;
	for ( std::vector< std::tuple< long, long, double > >::const_iterator iTrip = output.begin();
	      iTrip != output.end(); ++iTrip ) {
	    outputFile << std::get<0>(*iTrip) << "_" << std::get<1>(*iTrip) << "," << std::get<2>(*iTrip) << std::endl;
	}
	outputFile.close();
	std::cout << "Finished!" << std::endl;
    }
    catch ( std::exception& e ) {
	std::cerr << "Exception: " << e.what() << std::endl;
	return -1;
    }
    catch ( ... ) {
	std::cerr << "Unknown exception" << std::endl;
	return -1;
    }
    return 0;
}
