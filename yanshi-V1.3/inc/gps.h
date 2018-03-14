#ifndef GPS_H
#define GPS_H

extern int GPSComInit(char *DevPath);
extern void GpsProcess(void);
extern int initiateTrack(char *path);

typedef struct gps{
    int  week;              //GPS周
    double time;            //GPS秒
    double azimuth;         //偏航角
    double pitch;           //俯仰角
    double roll;            //横滚角
    double latitude;        //纬度
    double longitude;       //经度
    double high;        //高度
    double ve;              //东向速度
    double vn;              // 北向速度
    double vu;              //天向速度
    double base_line;        //基线长度
    int ant1;              //天线1卫星数
    int ant2;              //天线2卫星数
    int  status;
    double x;
    double y;
}*PGPS,GPSINFO;

typedef struct track{
    double  lat;
    double  lon;
    double  azimuth;
    double  speed;
    double  x;
    double  y;
}TRACK,*PTRACK;

extern PGPS pGps;
extern PTRACK   pTrack;
#endif
