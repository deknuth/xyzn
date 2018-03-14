#include "../inc/core.h"
unsigned int near_index;
unsigned int total;
float cacuFrontWheelAngle(double delta_angle, double aim_dist)
{
    float L = WHEELBASE;
    float temp = 0, temp1 = 0;
    temp = delta_angle * IPI;
    temp1  = atan(2 * L * sin(temp) / (aim_dist));
    return (temp1 * 180 / 3.141592627);     // radian convert to degree
}

double cacuAimObjDistance(unsigned int speed)
{
    double dist = 0,curvature;
    //    curvature =  cacuCurvature();
    dist = speed * 0.25;
    if(dist <= 3)
        dist = 3;
    else if(dist >= 7)
        dist = 7;
    return dist;
}

unsigned int seekOriPoint(PCOOR pCoor)     // find nearest point
{
    unsigned int index = 0;
    unsigned int nearest = 0;
    double temp = 10000;
    double distance,angle;
    double xx, yy;
    for(index = pCoor->cur_index; index<total; index++)
    {
        xx = (pTrack+index)->x + delta_x(index); //longitude 388011.84;//
        yy = (pTrack+index)->y + delta_y(index); //latitude 3123060.50;//
        angle = (pTrack+index)->azimuth - (pCoor->cur_azimuth);

        if(angle >180)
            angle -= 360;
        else if(angle < -180)
            angle += 360;
        // delta_angle_now = fabs(delta_angle_now); //keep the positive value

        distance = distOfPoint(pCoor->cur_x, pCoor->cur_y, xx, yy);
  //      printf("distance[%d]: %lf\t%lf\n",index,distance,angle);
    //    if(angle < 45 && angle > -45)
        {
            if(distance <= temp)
            {
                nearest = index;
                temp = distance;
            }
        }
    }
    return nearest;
}

double seekAimPoint(PCOOR pCoor)     // find aim point
{
    unsigned int index;
    double distance = 0;
    double dis_aim = 0;
    double xx = 0, yy = 0;
    double cur_x = 0, cur_y = 0;

    near_index = index = seekOriPoint(pCoor);      // nearest point index
    /*caculate the aim distance*/
    pCoor->n_index = index;
    distance = cacuAimObjDistance(pCoor->speed);  // confirm target distance

    _ker_printf("distance: %lf\n",distance);
    _ker_printf("nearest point index: %d\n",index);
    //    printf("XY: %.8lf\t%.8lf\n",track[index][1],track[index][2]);

    //search the point mathes the distance
    cur_x = (pTrack+index)->x +  delta_x(index); //longitude 388011.84;//
    cur_y = (pTrack+index)->y +  delta_y(index); //latitude 3123060.50;//

    while(1) //to get the aim point on the track
    {
        xx = (pTrack+index)->x +  delta_x(index); //longitude
        yy = (pTrack+index)->y +  delta_y(index); //latitude
        //  printf("distOfPoint(cur_x, cur_y, xx, yy) : %lf\n",distOfPoint(cur_x, cur_y, xx, yy));
        if(distOfPoint(cur_x, cur_y, xx, yy) >= distance)
            break;
        if(++index >= total) //maximun num
        {
            index = total;
            break;
        }
    }
    _ker_printf("aim point index: %d\n",index);
    pCoor->aim_index = index;
    /*caculate the real distance between current and aim points*/
    xx = (pTrack+index)->x  +  delta_x(index); //longitude
    yy = (pTrack+index)->y  +  delta_y(index); //latitude
    dis_aim = distOfPoint(cur_x, cur_y, xx, yy);//for control
    return dis_aim;
}

int followTrack(PCOOR pCoor)
{
    double  delta_angle_pos = 0, delta_angle_direction = 0, delta_angle = 0;
    float angle = 0;
    int steering_angle = 0;
    float verticalDist;
    double distance,aim_azimuth,aim_cur_angle;
    distance = seekAimPoint(pCoor);

    aim_azimuth = (pTrack+pCoor->aim_index)->azimuth;     // aim point azimuth
    aim_cur_angle =  cacuAimCurAzimuth(pCoor);          //caculate the angle of current and aim point

    //    printf("aim_cur_angle: %f\n",aim_cur_angle);
    //    printf("aim_azimuth: %f\n",aim_azimuth);
    /*caculate the delta angle between obj angle and current azimuth*/
    delta_angle_pos = delta_azimuth(pCoor->cur_azimuth,  aim_cur_angle);//PID
    delta_angle_direction = delta_azimuth(pCoor->cur_azimuth,  aim_azimuth);

    /*decided by the vertical distance*/
    verticalDist = verticalLineDist(pCoor);
  //  _ker_printf("verticalDist: %lf\n",verticalDist);
  //  _ker_printf("aim_dist_real: %lf\n",distance);
    _ker_printf("angle: %lf\t%lf\n",delta_angle_pos,delta_angle_direction);
  //  sprintf(temp,"%lf %lf\n",delta_angle_pos,delta_angle_direction);
   // fputs(temp,fps);
//    if((verticalDist > 1) || (verticalDist < -1))
        //  delta_angle = delta_angle_pos;
//        delta_angle = delta_angle_pos * 0.2+ delta_angle_direction * 0.8;
//    else
        //     delta_angle = delta_angle_direction;
    delta_angle = delta_angle_pos * 0.8 + delta_angle_direction * 0.05;

    // caculate the front wheel
    angle = cacuFrontWheelAngle(delta_angle, distance);
    if(fabs(delta_angle) < 1)
        steering_angle = 0;
    else
        steering_angle = (int)angle;

    _ker_printf("Control steering: %d\n",steering_angle<<4);
    return (steering_angle<<4);
}
