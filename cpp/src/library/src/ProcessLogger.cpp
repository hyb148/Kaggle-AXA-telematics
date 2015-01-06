#include "ProcessLogger.h"

#include <iostream>
#include <iomanip>
#include <string>

ProcessLogger::ProcessLogger( long numberOfTasks ):
  m_numberOfTasks( numberOfTasks ),
  m_completedTasks( 0 )
{}

void
ProcessLogger::taskEnded()
{
    std::lock_guard<std::mutex> lock( m_mutex );
    if ( m_completedTasks == m_numberOfTasks ) return;
    m_completedTasks++;
    if ( m_completedTasks > 1 ) std::cout << "\r";
    float percentage = 100.0 * m_completedTasks / m_numberOfTasks ;
    std::cout << "Tasks processed: " << std::fixed << std::setprecision(2) << percentage << "%";
    if ( m_completedTasks == m_numberOfTasks ) std::cout << std::endl;
    std::cout.flush();
}
