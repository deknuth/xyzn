#ifndef COORDINATES_H
#define COORDINATES_H

void CovertBLToXY(double latitude,double longitude,double centerLongitude,double *cx,double *cy,double *cr);
void WGS84ToGuass(double latitude,double longitude,double *x,double *y);
#endif // COORDINATETRANSFER_H
