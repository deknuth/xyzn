#include "../inc/core.h"
int output = 0;
void VehCtrl(unsigned char state,int angle)  //  Vehicle parameter control
{
    switch(state)
    {
    case 0:
        ccpParam->state = STOP;
        ccpParam->angle = STEER_CENTER_VALUE;
        ccpParam->throttle = 0;
        ccpParam->gear = 0;
        ccpParam->brake = 240;
        _ker_printf("Vehicle stop!\n");
        break;
    case 1:
        ccpParam->state = START;
        ccpParam->angle = angle;
        ccpParam->gear = 1;
        ccpParam->throttle = 1900;
        ccpParam->brake = 0;
        break;
    case 2:
        ccpParam->state = PAUSE;
        ccpParam->throttle = 0;
        ccpParam->brake = 240;
        _ker_printf("Vehicle pause!\n");
        break;
    default:
        break;
    }
}

