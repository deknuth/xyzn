#ifndef PUBLIC_H
#define PUBLIC_H
#include "follow.h"
extern int CreatPipe(void);
extern unsigned long GetFSize(const char *path);        // get file size
extern double distOfPoint(double x1, double y1, double x2, double y2);
extern double delta_x(double angle);
extern double delta_y(double angle);
extern double cacuAimCurAzimuth(PCOOR pCoor);
extern double delta_azimuth(double cur_azimuth, double aim_azimuth);
extern double verticalLineDist(PCOOR pCoor);
extern double includedAngle(PCOOR pCoor);
extern int _write(int fd, void *buffer, int length);
#define IPI 0.0174532925199433333333        // pi/180
#define DATA_PATH   "/home/lovenix/data/out_f.txt"
#endif
