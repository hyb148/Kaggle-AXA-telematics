#include <iostream>
#include <exception>

#include "DriverDataProcessing.h"

int main( int, char**) {
    try {
        std::string driverCompressedDir = "drivers_compressed_data";
        DriverDataProcessing dataProcessing( driverCompressedDir );
        
        std::vector< std::auto_ptr<Driver> > drivers = dataProcessing.loadAllData();
        
        std::cout << "Done. ";
        std::cin.get();
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
