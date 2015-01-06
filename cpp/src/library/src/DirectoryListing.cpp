#include "DirectoryListing.h"

#include <sstream>
#include <cstdlib>
#include <exception>


DirectoryListing::DirectoryListing( std::string targetDirectory ):
m_currentDirectory( targetDirectory )
{}

DirectoryListing&
DirectoryListing::setWorkingDirectory( const std::string& cwd ) {
    m_currentDirectory = cwd;
    return *this;
}

std::list<std::string>
DirectoryListing::directoryContent() const {
    std::list<std::string> result;
    
    std::string command = "ls " + m_currentDirectory;
    
    FILE* pipe = popen( command.c_str(), "r" );
    if (!pipe ) throw std::runtime_error( "Could not open pipe" );
    
    char buffer[128];
    std::string output = "";
    while ( !feof(pipe) ) {
        if ( fgets(buffer, 128, pipe ) ) {
            output += buffer;
        }
    }
    pclose(pipe);
    
    std::istringstream is(output);
    
    while (is) {
        
        std::string content;
        is >> std::skipws >> content >> std::ws;
        if ( content.empty() ) break;
        result.push_back(content);
    }
    
    return result;
}
