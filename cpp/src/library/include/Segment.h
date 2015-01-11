#ifndef SEGMENT_H
#define SEGMENT_H

#include <vector>
#include <utility>

class Segment{
public:
    // Constructor
    explicit Segment( const std::vector< std::pair<float, float> >& segmentCoordinates );
    
    // Destructor
    virtual ~Segment();
    
    // The speed values
    std::vector<double> speedValues() const;
    
    // The angular values
    std::vector<double> angularValues() const;
    
    // The distance travelled
    double travelLength() const;
    
    // The time duration
    long travelDuration() const;
    
private:
    // The point of origin
    std::pair<float, float> m_origin;

    // the velocity vectors
    std::vector< std::pair<float,float> > m_velocityVectors;
};

#endif
