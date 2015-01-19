#ifndef PROCESSLOGGER_H
#define PROCESSLOGGER_H

#include <mutex>
#include <string>

class ProcessLogger {
public:
    // Constructor initialised by the number of tasks needed
    ProcessLogger( long numberOfTasks,
		   std::string messagePrefix = "Tasks processed : " );
    
    // Signals that a task has successfully completed
    void taskEnded();
    
private:
    // The number of tasks to complete
    long m_numberOfTasks;
    
    // The number of completed tasks
    long m_completedTasks;

    // The message prefix
    std::string m_prefix;
    
    // The mutex to be used for locking
    std::mutex m_mutex;
};

#endif
