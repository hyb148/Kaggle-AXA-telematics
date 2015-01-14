#ifndef SEGMENT_H
#define SEGMENT_H

#include <vector>
#include <utility>
#include <tuple>

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

    // The acceleration values
    std::vector<double> accelerationValues() const;

    // The speed, acceleration and direction values as associated tuples
    std::vector< std::tuple<double,double,double> > speedAccelerationDirectionValues() const;
    
    // The distance travelled
    double travelLength() const;
    
    // The time duration
    long travelDuration() const;
    
    // The data points
    std::vector< std::pair<float,float> > dataPoints() const;
    
private:
    // The point of origin
    std::pair<float, float> m_origin;

    // the velocity vectors
    std::vector< std::pair<float,float> > m_velocityVectors;
};

#endif
