#include "../inc/core.h"

typedef struct cc{
    double p1_x;
    double p1_y;
    double p1_angle;
    double p2_x;
    double p2_y;
    double p2_angle;
    double p3_x;
    double p3_y;
    double p3_angle;
}*PCURVA,CURVA;         // Curvature struct

double cacuCurvature(PCURVA pCurva)
{
    double K = 0;
    double S = 0;
    double L = 0;
    double a,b,c,d,e,f;

    pCurva->p1_x = pCurva->p1_x + delta_x(pCurva->p1_angle); //longitude
    pCurva->p1_y = pCurva->p1_y + delta_y(pCurva->p1_angle); //latitude

    pCurva->p2_x = pCurva->p2_x + delta_x(pCurva->p2_angle); //longitude
    pCurva->p2_y = pCurva->p2_y + delta_y(pCurva->p2_angle); //latitude

    pCurva->p3_x = pCurva->p3_x + delta_x(pCurva->p3_angle); //longitude
    pCurva->p3_y = pCurva->p3_y + delta_y(pCurva->p3_angle); //latitude

    a = pCurva->p2_y - pCurva->p1_y;
    b = pCurva->p2_x - pCurva->p1_x;
    c = pCurva->p3_y - pCurva->p1_y;
    d = pCurva->p3_x - pCurva->p1_x;
    e = pCurva->p2_y - pCurva->p3_y;
    f = pCurva->p2_x - pCurva->p3_x;
    S=((pCurva->p2_x - pCurva->p1_x)*(pCurva->p3_y - pCurva->p1_y)-(pCurva->p3_x-pCurva->p1_x)*(pCurva->p2_y-pCurva->p1_y))/2;// triangle area
    L=(sqrt(pow((double) fabs(a),2.0)+pow((double) fabs(b),2.0))*(sqrt(pow((double) fabs(c),2.0)+pow((double) fabs(d),2.0)))*(sqrt(pow((double) fabs(e),2.0)+pow((double) fabs(f),2.0))));//L=A*B*C
    if(L == 0)
        return 0;
    else
    {
        K=(4*S)/L;      //curvature
        return K;
    }
}
