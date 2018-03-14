#include "../inc/core.h"

volatile unsigned char obstacle = 0;
double sinValue[36000] = {0};
double cosValue[36000] = {0};
volatile unsigned int sum = 0;
volatile unsigned int flag = 0;
struct timeval start;
struct timeval end;
static double VerDisCalcu(double x,double y,unsigned int index);

static int PackDecoder(unsigned char* buf);
struct data{
    unsigned int angle;
    unsigned int dis;
    unsigned char ref;  // reflectivity
}LidarData[24];

struct lidar{
    unsigned char valid;
    unsigned int horizon[256];
    unsigned int vertical[256];
    unsigned int hypotenuse[256];
}cluster[201];

int PackDecoder(unsigned char* buf)
{
    int i,j;
//	if(total == 0)
    //gettimeofday(&start,NULL);
    unsigned char temp[1000]={0};
    unsigned int angle = 0;
 //   long timer;
    int ver_dis = 0;
    int hor_dis = 0;
  //  memcpy(temp,buf,1000);

    for(i=0,j=0; i<12; i++)
    {
        memcpy(temp,&buf[i*100],100);
        if(temp[0]==0xFF && temp[1]==0xEE)
        {
            LidarData[j].angle = (temp[3]<<8) + temp[2];
            LidarData[j].dis = ((temp[5]<<8)+temp[4])<<1;
            LidarData[j].ref = temp[6];
            LidarData[++j].dis = ((temp[53]<<8)+temp[52])<<1;
            // LidarData[j].direct = (direct/100)+0.18;
            LidarData[j++].ref = temp[54];
        }
    }

    for(i=0,j=1; i<12; i++)
    {
        LidarData[j].angle = ((LidarData[j-1].angle+LidarData[j+1].angle)/2);
        j += 2;
        if(j > 21)
        {
            LidarData[j].angle = LidarData[j-1].angle>35970?30-(36000-LidarData[j-1].angle):LidarData[j-1].angle+30;
            break;
        }
    }

    for(i=0; i<24; i++)
    {
        if(LidarData[i].angle>27000 || LidarData[i].angle<9000)
        {
        //	printf("angle: %d\tdistance: %d\n",LidarData[i].angle,LidarData[i].dis);
            if(LidarData[i].dis != 0)
            {
                ver_dis = abs((int)(cosValue[LidarData[i].angle]*LidarData[i].dis));

                if(ver_dis < SAFE_DIS)
                {
                    if(LidarData[i].angle>27000)
                        angle = LidarData[i].angle - 27000;
                    else
                        angle = 9000 + LidarData[i].angle ;
                    hor_dis = abs((int)(sinValue[LidarData[i].angle]*LidarData[i].dis));
                    //printf("Ver: %d\tHor: %d\n",LidarData[i].dis,hor_dis);
                    if(hor_dis < HOR_MIN_DIS)
                    {
                        angle /= 100;
                        cluster[angle].horizon[cluster[angle].valid] = hor_dis;
                        cluster[angle].vertical[cluster[angle].valid] = ver_dis;
                        cluster[angle].hypotenuse[cluster[angle].valid] = LidarData[i].dis;
                        cluster[angle].valid++;
                        //printf("Angle: %d %d\t Horizon: %d\tVertical: %d\n",LidarData[i].angle,angle,hor_dis,ver_dis);
                    }
                }
            }
        }
      //  printf("angle: %.2f\tdistance: %f\t %f\t %d\t ref: %d\n",LidarData[i].direct,sin(j2h(LidarData[i].direct))*LidarData[i].distance,cos(j2h(LidarData[i].direct))*LidarData[i].distance,LidarData[i].distance,LidarData[i].reflectivity);
    }
    sum += 24;
    if(sum > 2000)
    {

        sum = 0;
        flag = 1;
    }
    return 0;
}

struct ob{
    int number;
    unsigned int wide[160];
    unsigned int hor[160];
    unsigned int ver[160];
    int angle[160];     // center angle
    unsigned int freq[160];
    unsigned int hyp[160];
}obst;  //obstacle list

int LidarCluster(void)
{
    double x=0,y=0;
    int i,j,k=0;
    int last = 0;
    int last_ver = 0;
    int wide = 0;
    int angle = 0;
    int number = 0;
    double adj = 0;
    double opp = 0;
    double a = 0;
    unsigned long vertical = 0;
    unsigned long horizon = 0;
    unsigned long hypotenuse = 0;
    double distance = 0;
    for(;;)
    {
        if(1 == flag)
        {
            memset(&obst,0x00,sizeof(obst));
            gettimeofday(&start,NULL);
            for(i=1; i<200; i++)
            {
                if(cluster[i].valid > 0)
                {
                    for(j=0; j<cluster[i].valid; j++)       // every degree
                    {
                        vertical += cluster[i].vertical[j];     // vertical distance sum
                        horizon += cluster[i].horizon[j];
                        hypotenuse += cluster[i].hypotenuse[j];
                        number++;                               // point number
                    }
                    if(number)
                    {
                        if(0==last)
                        {
                            last_ver = vertical/number;
                            angle += i;
                            last = i;
                            wide++;
                        }
                        else
                        {
                            if(abs(last-i)<2 && abs(last_ver-(vertical/number)) < 180)      //vertical separate
                            {
                          //      printf("%d\n",last_ver-(vertical/number));
                                last_ver = vertical/number;
                                angle += i;
                                last = i;
                                wide++;
                            }
                            else    // calculate  para
                            {
                              //  if(wide)
                                {
                                    angle /= wide;
                                    //printf("wide: %d\t%d\t%d\n",wide,angle,number);
                                    obst.ver[obst.number] = vertical/number;
                                    obst.hor[obst.number] = horizon/number;
                                    obst.hyp[obst.number] = hypotenuse/number;
                                    obst.angle[obst.number] = angle;
                                    obst.wide[obst.number] = wide;
                                    obst.freq[obst.number] = number;
                                    obst.number++;
                                }
                                angle = wide = last = number = last_ver = 0;
                                vertical = horizon = hypotenuse = 0;
                            }
                        }
                    }
                    //    printf("angle[%d]: %d\t%d\n",i,cluster[i].horizon[j],cluster[i].vertical[j]);
                 //   printf("angle[%d]: %d\t%d\n",i,cluster[i].valid,cluster[i].horizon[0]);
                }
            }
            if(number)
            {
                angle /= wide;
                //printf("wide1: %d\t%d\t%d\n",wide,angle,horizon/number);
                obst.ver[obst.number] = vertical/number;
                obst.hor[obst.number] = horizon/number;
                obst.hyp[obst.number] = hypotenuse/number;
                obst.angle[obst.number] = angle;
                obst.wide[obst.number] = wide;
                obst.freq[obst.number] = number;
               // printf("number: %d\t hyp: %d\n",number,obst.hyp[obst.number]);
                obst.number++;
            }
            if(obst.number)
            {
                for(k=0; k<obst.number; k++)
                {
                    if(obst.freq[k] > 10)
                    {
                     //   a = (double)obst.hyp[k]/1000.0;
                     //   c = sqrt(1.96 + a*a - 2.8*a*cos(180-abs(90-obst.angle[k])));
                        adj = LIDAR_OFFSET+(double)obst.ver[k]/1000.0;
                        opp = (double)obst.hor[k]/1000.0;
                        a = atan(opp/adj);
                        a = h2j(a);
                      //  _lid_printf("adj: %lf\t opp: %lf\n",adj,opp);
                      //  x = pGps->x+LIDAR_OFFSET-fabs(cos(obst.angle[k])*((double)obst.hyp[k]/1000.0));
                      //  y = pGps->y+LIDAR_OFFSET-fabs(sin(obst.angle[k])*((double)obst.hyp[k]/1000.0));

                        _lid_printf("Dela a: %lf Azimuth: %lf\tangle: %d\n",a,pGps->azimuth,obst.angle[k]);
                        if((obst.angle[k]-90) > 0)
                            a += pGps->azimuth;
                        else
                            a = pGps->azimuth-a;
                        if((int)a < 0)
                           a += 360;
                        else if((int)a > 360)
                            a -= 360;
                         _lid_printf("Dela a:%lf\n",a);
                        switch(((int)a)/90)
                        {
                        case 0:
                            x = pGps->x+opp;
                            y = pGps->y+adj;
                            break;
                        case 1:
                            x = pGps->x+opp;
                            y = pGps->y-adj;
                            break;
                        case 2:
                            x = pGps->x-opp;
                            y = pGps->y-adj;
                            break;
                        case 3:
                            x = pGps->x-opp;
                            y = pGps->y+adj;
                            break;
                        default:
                            break;
                        }

                        /*
                        switch(((int)pGps->azimuth)/90)
                        {
                        case 0:
                            x = pGps->x+LIDAR_OFFSET+cos(j2h(obst.angle[k]))*((double)obst.hyp[k]/1000.0);
                            y = pGps->y+LIDAR_OFFSET+sin(j2h(obst.angle[k]))*((double)obst.hyp[k]/1000.0);
                            break;
                        case 1:
                            x = pGps->x+LIDAR_OFFSET+cos(j2h(obst.angle[k]))*((double)obst.hyp[k]/1000.0);
                            y = pGps->y-LIDAR_OFFSET-sin(j2h(obst.angle[k]))*((double)obst.hyp[k]/1000.0);
                            break;
                        case 2:
                            x = pGps->x-LIDAR_OFFSET-cos(j2h(obst.angle[k]))*((double)obst.hyp[k]/1000.0);
                            y = pGps->y-LIDAR_OFFSET-sin(j2h(obst.angle[k]))*((double)obst.hyp[k]/1000.0);
                            break;
                        case 3:
                            x = pGps->x-LIDAR_OFFSET-cos(j2h(obst.angle[k]))*((double)obst.hyp[k]/1000.0);
                            y = pGps->y+LIDAR_OFFSET+sin(j2h(obst.angle[k]))*((double)obst.hyp[k]/1000.0);
                            break;
                        default:
                            break;
                        }
                        */
                        distance = VerDisCalcu(x,y,near_index);
                        _lid_printf("cx: %lf %lf\tcy: %lf %lf point: %d\n",pGps->x,(pTrack+near_index)->x,pGps->y,(pTrack+near_index)->y,near_index);
                        _lid_printf("distance: %lf\tadj: %lf\topp: %lf\n",distance,adj,opp);
                        //_lid_printf("distance: %lf\t%lf\t%lf\t%lf\t%lf\t%d\n",distance,pGps->x,pGps->y,pGps->azimuth,(double)obst.hyp[k]/1000.0,near_index);
                        if((int)(distance*1000) < HOR_MIN_DIS)
                        {
                            obstacle = 1;
                            break;
                        }
                        else
                            obstacle = 0;
                    }
                    else
                        obstacle = 0;
                }

                if(obstacle)
                {
                    lprintf(lfd,INFO,"angle:%d, hor:%d, ver:%d, wide:%d, freq:%d, steer:%d",obst.angle[k],obst.hor[k],obst.ver[k],obst.wide[k],obst.freq[k],output);
                 //   _lid_printf("obstacle[%d]: %d   %d   %d   %d   %d  %d\n",k,obst.angle[k],obst.hor[k],obst.ver[k],obst.wide[k],obst.freq[k],output);
                 //   _lid_printf("weight: %d  %d\n",weight,obst.angle[k]);
                  }
            }
            else
                obstacle = 0;
            flag = sum = 0;
            last = angle = wide = number = last_ver = 0;
            vertical = horizon = hypotenuse = 0;
            gettimeofday(&end,NULL);
            memset(cluster,0x00,sizeof(cluster));
//            _lid_printf("timer: %ldus\n",end.tv_usec-start.tv_usec);//(end.tv_sec-start.tv_sec);
        }
    }
}

int LidarClient(void)
{
    unsigned char buf[2048] = { 0 };
    struct sockaddr_in local,client;
    socklen_t len = sizeof(client);
    int i,sock,rLen;
    float step = 0.00;
    memset(cluster,0x00,sizeof(cluster));
    for(i=0; i<36000; i++)
    {
        sinValue[i] = sin(j2h(step));
        cosValue[i] = cos(j2h(step));
        step += 0.01;
    }

    if((sock = socket(AF_INET,SOCK_DGRAM,0)) < 0)
    {
         _lid_printf("socket");
         return -1;
    }
    local.sin_family = AF_INET;
    local.sin_port = htons(2368);
    local.sin_addr.s_addr = inet_addr("192.168.1.125");
    if(bind(sock,(struct sockaddr*)&local,sizeof(local))<0)
    {
        _lid_printf("bind");
        return(-1);
    }
    while(1)
    {
        rLen = recvfrom(sock,buf,sizeof(buf)-1,0,(struct sockaddr*)&local,&len);
        if(rLen < 0)
        {
            _lid_printf("recvfrom");
            return -1;
        }
        else if(rLen == PACKET_SIZE)
            PackDecoder(buf);
    }
    return 0;
}

double VerDisCalcu(double x,double y,unsigned int index)       // Vertical distance calculation
{
    unsigned int i = 0;
    unsigned int end = 0;
    unsigned int min_index=0;
    double min = 10000.0;
    double distance = 0;
    if(total-index > 60)        // find 60 point
        end = index+60;
    else
        end = total;
    for(i=index; i<end; i++)
    {
        distance = distOfPoint(x, y, (pTrack+i)->x, (pTrack+i)->y);
        if(distance <= min)
        {
//            printf("dis: %lf\n",distance);
            if((int)(distance*1000) < HOR_MIN_DIS)
            {
                min = distance;
                min_index = i;
                break;
            }
            min = distance;
            min_index = i;
        }
    }
    _lid_printf("Min point index: %d\n",i);
    return min;
}
