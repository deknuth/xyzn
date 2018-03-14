#ifndef DECISIONMAKE_H
#define DECISIONMAKE_H

void kernel(void);

// vector <vector<double> > track;  /*0:number, 1:x_longitude 2:y_latitude, 3:direction, 4:vehicle speed, 5:track segment, 6. speical point*/

struct PointCoordinate
{
    double p1_x; /*rectangle of the vehicle*/
    double p1_y;
    double p2_x;
    double p2_y;
    double p3_x;
    double p3_y;
    double p4_x;
    double p4_y;
    float closest_p_dist; // distance between cloest point and (center point of vehicle) in vehicle coordiance.
    unsigned int id;
    unsigned int classficiation;
};

#endif
