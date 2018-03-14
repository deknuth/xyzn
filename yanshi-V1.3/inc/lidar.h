#ifndef LIDAR_H
#define LIDAR_H
#define SAFE_DIS 8000
#define HOR_MIN_DIS 800
#define PACKET_SIZE 1206
#define j2h(x) (3.1415926*(x)/180.0)
#define h2j(x) ((x)*180/3.1415926)
#define LIDAR_OFFSET  1.4             // x=sin(45)*l  y=sin(45)*l  l=(gps to lidar center)

extern int LidarClient(void);
extern int LidarCluster(void);
extern volatile unsigned char obstacle;
#endif // LIDAR_H
