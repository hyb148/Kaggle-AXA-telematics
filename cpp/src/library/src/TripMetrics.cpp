#include "TripMetrics.h"
#include <ostream>

TripMetrics::TripMetrics( long tripId,
                          const std::vector<double>& values ):
  m_tripId( tripId ),
  m_driverId(0),
  m_values( values )
{}

TripMetrics::~TripMetrics()
{}

const std::vector< std::string >&
TripMetrics::descriptions() const
{
    static const std::vector< std::string > descriptions = {
        "ZeroSegments",
        "FewPoints",
        "log10(1+TripDuration)",
        "log10(1+TripLength)",
        "log10(0.1+speedP25)",
        "log10(0.1+speedP50)",
        "log10(0.1+speedP75)",
        "log10(0.1+speedP95)",
        "log10(-accelerationP05)",
        "log10(-accelerationP25)",
        "log10(accelerationP75)",
        "log10(accelerationP95)",
        "log10(-directionP05)",
        "log10(-directionP25)",
        "log10(directionP75)",
        "log10(directionP95)",
        "log10(-speedXaccelerationP05)",
        "log10(-speedXaccelerationP25)",
        "log10(speedXaccelerationP75)",
        "log10(speedXaccelerationP95)",
        "log10(0.001+totalDirectionChange)"
    };
    return descriptions;
}


std::ostream&
TripMetrics::writeDescriptions( std::ostream& out ) const
{
    const std::vector< std::string >& descriptions = this->descriptions();
    for (size_t i = 0; i < descriptions.size(); ++i ) {
        if (i > 0 ) out << " ";
        out << descriptions[i];
    }
    return out;
}



std::ostream&
TripMetrics::writeValues( std::ostream& out ) const
{
    for (size_t i = 0; i < m_values.size(); ++i ) {
        if (i > 0 ) out << " ";
        out << m_values[i];
    }
    return out;
}


std::ostream&
operator<<( std::ostream& os,
            const TripMetrics& metrics )
{
    metrics.writeValues( os );
    return os;
}
