#include "TripMetrics.h"

TripMetrics::TripMetrics():
    travelDuration(0),
    travelLength(0),
    lengthToDistance(0),
    speed_p25(0),
    speed_q13(0),
    speed_m(0),
    speed_p95(0),
    acceleration_p05(0),
    acceleration_q25(0),
    acceleration_q13(0),
    acceleration_m(0),
    acceleration_p95(0),
    direction_p05(0),
    direction_q25(0),
    direction_q13(0),
    direction_m(0),
    direction_p95(0),
    r2_sa(0),
    r2_sd(0),
    r2_ad(0)
{}

TripMetrics::~TripMetrics()
{}

