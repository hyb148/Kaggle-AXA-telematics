#include <iostream>
#include <fstream>
#include <sstream>
#include <exception>

#include "DriverDataProcessing.h"
#include "Segment.h"

class CppToPythonPipe
{
public:
    explicit CppToPythonPipe( const std::string& inputFileName,
			      const std::string& outputFileName,
			      std::vector< std::auto_ptr<Driver> >& drivers,
			      DriverDataProcessing& dataProcessing ):
    m_inputFileName(inputFileName),
    m_outputFileName(outputFileName),
    m_drivers( drivers ),
    m_dataProcessing( dataProcessing )
    {}
    
    virtual ~CppToPythonPipe() {}

private:
    Driver* driver( int driverId ) const {
	std::vector< std::auto_ptr<Driver> >::iterator iDriver  = m_drivers.begin();
	while (  iDriver != m_drivers.end() ) {
	    if ( (*iDriver)->id() == driverId ) break;
	    ++iDriver;
	}
	if (iDriver == m_drivers.end() ) return 0;
	return iDriver->get();
    }

    const Trip* trip( int driverId, int tripId ) const {
	Driver* driver = this->driver( driverId );
	if ( driver == 0 ) return 0;
	const std::vector<Trip>& trips = driver->trips();
	std::vector<Trip>::const_iterator iTrip = std::find( trips.begin(), trips.end(), tripId );
	if (iTrip == trips.end() ) return 0;
	return &(*iTrip);
    }
    
public:
    bool processCommands() {
        std::ifstream inputPipe( m_inputFileName );
        char buf[256];
        std::memset(buf, 0, sizeof(buf));
        inputPipe.read( buf, sizeof(buf) );
        inputPipe.close();

        std::istringstream isInput( buf );
        
        std::string command;
        isInput >> command;
        if (command == "exit") {
            return false;
        }
        
        int driverId, tripId;
        isInput >> driverId >> tripId;

        // Depending on the command name we can decide what data to send to the pipe
        if ( command == "rawdata" ) {
	    const Trip* trip = this->trip( driverId, tripId );
	    if ( trip == 0 ) return false;            
            std::ofstream outputPipe( m_outputFileName );
            const std::vector< std::pair< float, float > >& rawData = trip->rawData();
            outputPipe << rawData.size() << std::endl;
            for ( std::vector< std::pair< float, float > >::const_iterator iPoint = rawData.begin(); iPoint != rawData.end(); ++iPoint )
                outputPipe << iPoint->first << " " << iPoint->second << std::endl;
            outputPipe.close();
        }
        else if ( command == "segments" ) {
	    const Trip* trip = this->trip( driverId, tripId );
	    if ( trip == 0 ) return false;            
            std::ofstream outputPipe( m_outputFileName );
            const std::vector< Segment* >& segments = trip->segments();
            outputPipe << segments.size() << std::endl;
            for ( std::vector< Segment* >::const_iterator iSegment = segments.begin();
                 iSegment != segments.end(); ++iSegment ) {
                Segment& segment = **iSegment;
                std::vector< std::pair<float,float> > segmentRawData = segment.dataPoints();
                outputPipe << segmentRawData.size() << std::endl;
                for ( std::vector< std::pair< float, float > >::const_iterator iPoint = segmentRawData.begin();
		      iPoint != segmentRawData.end(); ++iPoint )
                    outputPipe << iPoint->first << " " << iPoint->second << std::endl;
            }
	    outputPipe.close();
        }
        else if ( command == "speed" ) {
	    const Trip* trip = this->trip( driverId, tripId );
	    if ( trip == 0 ) return false;
            std::ofstream outputPipe( m_outputFileName );
	    std::vector<double> values = trip->speedValues();
            outputPipe << values.size() << std::endl;
            for ( std::vector< double >::const_iterator iValue = values.begin();
                 iValue != values.end(); ++iValue ) {
                outputPipe << *iValue << std::endl;
            }
	    outputPipe.close();
        }
	else if ( command == "acceleration" ) {
	    const Trip* trip = this->trip( driverId, tripId );
	    if ( trip == 0 ) return false;
            std::ofstream outputPipe( m_outputFileName );
	    std::vector<double> values = trip->accelerationValues();
            outputPipe << values.size() << std::endl;
            for ( std::vector< double >::const_iterator iValue = values.begin();
                 iValue != values.end(); ++iValue ) {
                outputPipe << *iValue << std::endl;
            }
	    outputPipe.close();
        }
	else if ( command == "direction" ) {
	    const Trip* trip = this->trip( driverId, tripId );
	    if ( trip == 0 ) return false;            
            std::ofstream outputPipe( m_outputFileName );
	    std::vector<double> values = trip->directionValues();
            outputPipe << values.size() << std::endl;
            for ( std::vector< double >::const_iterator iValue = values.begin();
                 iValue != values.end(); ++iValue ) {
                outputPipe << *iValue << std::endl;
            }
	    outputPipe.close();
        }
	else if ( command == "speedAccelerationDirection" ) {
	    const Trip* trip = this->trip( driverId, tripId );
	    if ( trip == 0 ) return false;            
            std::ofstream outputPipe( m_outputFileName );
	    std::vector< std::tuple<double,double,double> > values = trip->speedAccelerationDirectionValues();
            outputPipe << values.size() << std::endl;
            for ( std::vector< std::tuple<double,double,double> >::const_iterator iValue = values.begin();
                 iValue != values.end(); ++iValue ) {
                outputPipe << std::get<0>( *iValue) << " " << std::get<1>( *iValue) << " " << std::get<2>( *iValue) << std::endl;
            }
	    outputPipe.close();
        }
	else if ( command == "speedQuantiles" ) {
	    const Trip* trip = this->trip( driverId, tripId );
	    if ( trip == 0 ) return false;            
            std::ofstream outputPipe( m_outputFileName );
	    std::vector<double> values = trip->speedQuantiles();
            outputPipe << values.size() << std::endl;
            for ( std::vector< double >::const_iterator iValue = values.begin();
                 iValue != values.end(); ++iValue ) {
                outputPipe << *iValue << std::endl;
            }
	    outputPipe.close();
        }
	else if ( command == "accelerationQuantiles" ) {
	    const Trip* trip = this->trip( driverId, tripId );
	    if ( trip == 0 ) return false;            
            std::ofstream outputPipe( m_outputFileName );
	    std::vector<double> values = trip->accelerationQuantiles();
            outputPipe << values.size() << std::endl;
            for ( std::vector< double >::const_iterator iValue = values.begin();
                 iValue != values.end(); ++iValue ) {
                outputPipe << *iValue << std::endl;
            }
	    outputPipe.close();
        }
	else if ( command == "directionQuantiles" ) {
	    const Trip* trip = this->trip( driverId, tripId );
	    if ( trip == 0 ) return false;            
            std::ofstream outputPipe( m_outputFileName );
	    std::vector<double> values = trip->directionQuantiles();
            outputPipe << values.size() << std::endl;
            for ( std::vector< double >::const_iterator iValue = values.begin();
                 iValue != values.end(); ++iValue ) {
                outputPipe << *iValue << std::endl;
            }
	    outputPipe.close();
        }
        else if ( command == "drivers") {
            std::ofstream outputPipe( m_outputFileName );
            outputPipe << m_drivers.size() << std::endl;
            for ( std::vector< std::auto_ptr<Driver> >::const_iterator iDriver = m_drivers.begin(); iDriver != m_drivers.end(); ++iDriver )
                outputPipe << (*iDriver)->id() << std::endl;
            outputPipe.close();
        }
        else if ( command == "trips") {            
            Driver* driver = this->driver( driverId );
	    if ( driver == 0 ) return false;
            const std::vector<Trip>& trips = driver->trips();
            
            std::ofstream outputPipe( m_outputFileName );
            outputPipe << trips.size() << std::endl;
            for ( std::vector<Trip>::const_iterator iTrip = trips.begin(); iTrip != trips.end(); ++iTrip )
                outputPipe << iTrip->id() << std::endl;
            outputPipe.close();
        }
	else if ( command == "travelDuration" ) {
	    const Trip* trip = this->trip( driverId, tripId );
	    if ( trip == 0 ) return false;            
            std::ofstream outputPipe( m_outputFileName );
	    outputPipe << trip->travelDuration() << std::endl;
	    outputPipe.close();
	}
	else if ( command == "travelLength" ) {
	    const Trip* trip = this->trip( driverId, tripId );
	    if ( trip == 0 ) return false;            
            std::ofstream outputPipe( m_outputFileName );
	    outputPipe << trip->travelLength() << std::endl;
	    outputPipe.close();
	}
	else if ( command == "distanceOfEndPoint" ) {
	    const Trip* trip = this->trip( driverId, tripId );
	    if ( trip == 0 ) return false;            
            std::ofstream outputPipe( m_outputFileName );
	    outputPipe << trip->distanceOfEndPoint() << std::endl;
	    outputPipe.close();
	}
	else if ( command == "allTripMetrics" ) {
	    std::vector< TripMetrics > outputData;
	    m_dataProcessing.produceTripMetrics( outputData );
	    std::ofstream outputPipe( m_outputFileName );
	    outputPipe << outputData.size() << std::endl;
	    for ( std::vector< TripMetrics >::const_iterator iTripData = outputData.begin();
		  iTripData != outputData.end(); ++iTripData ) {
		outputPipe << *iTripData << std::endl;
	    }
	    outputPipe.close();
	}
        else {
            std::cout << "Unknown Command: \"" << command << "\"" << std::endl;
            return false;
        }
        
        return true;
    }
    
private:
    std::string m_inputFileName;
    std::string m_outputFileName;
    std::vector< std::auto_ptr<Driver> >& m_drivers;
    DriverDataProcessing& m_dataProcessing;
};




int main( int, char**) {
    try {
        std::string driverCompressedDir = "drivers_compressed_data";
        DriverDataProcessing dataProcessing( driverCompressedDir );
        
        std::cout << "Loading and preprocessing the data" << std::endl;
        std::vector< std::auto_ptr<Driver> > drivers = dataProcessing.loadAllData();

        std::cout << "Ready for receing commands." << std::endl;

        CppToPythonPipe pipe( "pythontocpppipe", "cpptopythonpipe", drivers, dataProcessing );
        
        while( pipe.processCommands() );
        
        std::cout << "Exiting " << std::endl;
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
