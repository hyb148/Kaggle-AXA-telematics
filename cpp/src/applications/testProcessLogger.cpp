#include <iostream>
#include <exception>
#include <thread>
#include <chrono>

#include "ProcessLogger.h"

int main( int, char**) {
    try {
        std::cout << "Starting ... " << std::endl;
        ProcessLogger log(10);
        
        for (int i = 0; i < 12; ++i ) {
            log.taskEnded();
            std::this_thread::sleep_for (std::chrono::seconds(1));
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
