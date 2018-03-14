#ifndef FOLLOW_H
#define FOLLOW_H
extern unsigned int total;
extern unsigned int near_index;
typedef struct se{
    double x;
    double y;
    double azimuth;
    unsigned int speed;
    unsigned int index;
}*PSELF,SELF;

typedef struct pp{
    double aim_x;
    double aim_y;
    double aim_azimuth;
    unsigned int aim_index;     // aim point index
    double cur_x;
    double cur_y;
    double cur_azimuth;
    unsigned int cur_index;
    int speed;
    unsigned int n_index;    // nearest point index
}*PCOOR,COOR;

extern int followTrack(PCOOR pCoor);
extern float cacuFrontWheelAngle(double delta_angle, double aim_dist);
#endif // FOLLOW_H
