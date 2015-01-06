#ifndef PROCESSLOGGER_H
#define PROCESSLOGGER_H

#include <mutex>

class ProcessLogger {
public:
    // Constructor initialised by the number of tasks needed
    explicit ProcessLogger( long numberOfTasks);
    
    // Signals that a task has successfully completed
    void taskEnded();
    
private:
    // The number of tasks to complete
    long m_numberOfTasks;
    
    // The number of completed tasks
    long m_completedTasks;
    
    // The mutex to be used for locking
    std::mutex m_mutex;
};

#endif
