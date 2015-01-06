#ifndef DIRECTORYLISTING_H
#define DIRECTORYLISTING_H

#include <string>
#include <list>

class DirectoryListing {
public:
    DirectoryListing( std::string targetDirectory = "." );
    
    DirectoryListing& setWorkingDirectory( const std::string& cwd );
    
    std::list<std::string> directoryContent() const;

private:
    std::string m_currentDirectory;
};

#endif
