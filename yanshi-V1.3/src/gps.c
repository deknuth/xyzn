#include "../inc/core.h"

PGPS pGps;
PTRACK   pTrack;
static int DataProcess(char *buf,int len);

void GpsProcess(void)
{
    unsigned char temp;
    int count,start,len;
    fd_set readset; // 设置文件描述符集
    struct timeval tv;
    int MaxFd,nFd; // 文件描述符个数
    char buf[512] = {0};
    start = count = 0;
    do{
        FD_ZERO( &readset); // 文件描述符置0
        if (gfd >= 0)
            FD_SET(gfd, &readset);
        MaxFd = gfd + 1; // 最大文件描述符数
        tv.tv_sec = 4;
        tv.tv_usec = 0;
        do {
            nFd = select(MaxFd, &readset, 0, 0, &tv); // 检测状态
        } while (nFd==-1 && errno==EINTR); // 是否中断
        if (nFd <= 0)
        {
            _gps_printf("select gfd error: %d\n",nFd);
            gfd = -1;
        }
        else if(FD_ISSET(gfd, &readset)) // FD_ISSET检测fd是否设置
        {
            if((len=read(gfd, &temp, 1))>0)
            {
                if(temp == '$')			// pack head
                    start = 1;
                else if(temp == 0x0A)
                {
                    // _gps_printf("buf: %s\n",buf);
                    if(DataProcess(buf,count) == 0)
                        break;
                    else
                        memset(buf,0x00,count);
                    count = start = 0;
                }
                if(start)
                {
                    buf[count++] = temp;
                    if(count > 511)
                        count = 0;
                }
            }
            else
            {
                //        _gps_printf("Read len: %d\n",len);
                break;
            }
        }
    }while(gfd >= 0);
    _gps_printf("GPS read error!\n");
}

/***@brief  设置串口通信速率
 *@param  fd     类型 int  打开串口的文件句柄
 *@param  speed  类型 int  串口速度
 *@return  void
 */
#define BAUDRATE B115200
int GPSComInit(char *DevPath)
{
    int fd;
    struct termios newtio;
    fd = open(DevPath, O_RDWR|O_NOCTTY|O_NDELAY);
    if (fd == -1)
    {
        _gps_printf("can't open GPS port!\n");
        return (-1);
    }
    tcgetattr(fd,&newtio);
    bzero(&newtio,sizeof(newtio));
    //setting c_cflag
    newtio.c_cflag |= (CLOCAL|CREAD);
    newtio.c_cflag &= ~PARENB;
    newtio.c_cflag &= ~PARODD;
    newtio.c_cflag &= ~CSTOPB;
    newtio.c_cflag &= ~CSIZE;
    newtio.c_cflag |= CS8;
    newtio.c_oflag |= OPOST;
    //newtio.c_lflag = 0;
    //newtio.c_cc[VTIME] = 5;
    //newtio.c_cc[VMIN] = 0;
    //setting c_iflag
    newtio.c_iflag &=~ (IXON|IXOFF|IXANY);
    cfsetispeed(&newtio,BAUDRATE);
    cfsetospeed(&newtio,BAUDRATE);
    //printf("speed=%d\n",baud);
    tcflush(fd, TCIFLUSH);
    tcsetattr(fd,TCSANOW,&newtio);
    return(fd);
}

int DataProcess(char *buf,int len)
{
    int index = 0,i;
    char temp[15] = {0};
    if(len>23 && (kmp(buf,"$GPFPD")==0))
    {
        buf += 7;
        for(i=0;;i++)
        {
            index = kmp(buf,",");
            if(index <= 0 || index > 15)
                break;
            memcpy(temp,buf,index);

            switch(i)
            {
            case 0:                 //GPS周(wwww)
                pGps->week = atoi(temp);
                break;
            case 1:                               //GPS秒(ssssss.sss)
                pGps->time = strtod(temp,NULL);
                break;
            case 2:                                 //偏航角(hhh.hh)
                pGps->azimuth = strtod(temp,NULL);
                break;
            case 3:                               //俯仰角(+/-pp.pp)
                pGps->pitch = strtod(temp,NULL);
                break;
            case 4:                              //横滚角(+/-rrr.rr)
                pGps->roll = strtod(temp,NULL);
                break;
            case 5:         //纬度(+/-ll.lllllll)(‘+’-北纬，‘-’-南纬)
                pGps->latitude = strtod(temp,NULL);
                break;
            case 6:         //经度(+/-lll.lllllll)(‘+’-东经，‘-’-西经)
                pGps->longitude = strtod(temp,NULL);
                break;
            case 7:                      //高度(+/-aaaaa.aa)  单位米
                pGps->high = strtod(temp,NULL);
                break;
            case 8:                //东向速度(+/-eee.eee) 单位：米/秒
                pGps->ve = strtod(temp,NULL);
                break;
            case 9:               //北向速度(+/-nnn.nnn) 单位：米/秒
                pGps->vn = strtod(temp,NULL);
                break;
            case 10:               //天向速度(+/-uuu.uuu) 单位：米/秒
                pGps->vu = strtod(temp,NULL);
                break;
            case 11:                       //基线长度(bb.bbb) 单位：米
                pGps->base_line = strtod(temp,NULL);
                break;
            case 12:                                //天线1卫星数(nn)
                pGps->ant1 = atoi(temp);
                break;
            case 13:                               //天线2卫星数(nn)
                pGps->ant2 = atoi(temp);
                break;
            default:
                break;
            }
            memset(temp,0x00,15);
            buf += (index+1);
        }
        buf++;
        if(*buf > 64)
            pGps->status = (*buf)-55;
        else
            pGps->status = (*buf)-48;

    }
    _gps_printf("Get coordinate: %.8lf\t %.8lf\t %lf\t %lf\n",pGps->latitude,pGps->longitude,pGps->azimuth,pGps->vn);
    //  CovertBLToXY(latitude, longitude,114, &x_cor, &y_cor, &c_cor);

    switch(pGps->status)
    {
    case 0:
        _gps_printf("GPS状态: 初始化\n");
        break;
    case 1:
        _gps_printf("GPS状态: 粗对准\n");
        break;
    case 2:
        _gps_printf("GPS状态: 精对准\n");
        break;
    case 3:
        _gps_printf("GPS状态: GPS定位\n");
        break;
    case 4:
        _gps_printf("GPS状态: GPS定向\n");
        break;
    case 5:
        _gps_printf("GPS状态: RTK\n");
        break;
    case 6:
        _gps_printf("GPS状态: DMI组合\n");
        break;
    case 7:
        _gps_printf("GPS状态: DMI标定\n");
        break;
    case 8:
        _gps_printf("GPS状态: 纯惯性\n");
        break;
    case 9:
        _gps_printf("GPS状态: 零速校正\n");
        break;
    case 10:
        _gps_printf("GPS状态: VG模式\n");
        break;
    case 11:
        _gps_printf("GPS状态: 差分定向\n");
        break;
    case 12:
        _gps_printf("GPS状态: 动态对准\n");
        break;
    default:
        _gps_printf("GPS状态: 数据错误\n");
        break;
    }
    return 1;
}

int initiateTrack(char *path)
{
    int i = 0;
    double x,y;
    unsigned long  fsize = 0;
    FILE* fp;
    char temp[128] = {0};
    char value[16] = {0};
    char *p = NULL;
    fsize = GetFSize(path);        // get file size
    if(fsize < 512)
    {
        _ker_printf("Track data too small!\n");
        return 0;
    }
    if((fp=fopen(path, "rb")) == NULL)
    {
        _ker_printf("Not foud gps track data!\n");
        return 0;
    }
    for(i=0;;i++)
    {
        p = fgets(temp, 128, fp);
        if(fsize - ftell(fp) < 20)
            break;
    }
    pTrack = (PTRACK)malloc(sizeof(TRACK)*(i+1));
    fseek(fp,0,SEEK_SET);       // move to start
    for(i=0;;i++)
    {
        p = fgets(temp, 128, fp);
        _ker_printf("%s\n",temp);

        //sscanf(temp,"%16[^,]",value);
        sscanf(temp,"%*[^,],%16[^,]",value);
//        track[i][0] = strtod(value,NULL);
        (pTrack+i)->lon = strtod(value,NULL);
        memset(value, 0x00, 16);

        sscanf(temp,"%*[^,],%*[^,],%16[^,]",value);
 //       track[i][1] = strtod(value,NULL);
        (pTrack+i)->lat = strtod(value,NULL);
        memset(value, 0x00, 16);

        sscanf(temp,"%*[^,],%*[^,],%*[^,],%16[^,]",value);
//        track[i][2] = strtod(value,NULL);
        (pTrack+i)->azimuth = strtod(value,NULL);
        memset(value, 0x00, 16);

        sscanf(temp,"%*[^,],%*[^,],%*[^,],%*[^,],%16[^,]",value);
//        track[i][3] = strtod(value,NULL);
        (pTrack+i)->speed = strtod(value,NULL);
        memset(value, 0x00, 16);

/*
        sscanf(temp,"%*[^,],%*[^,],%*[^,],%*[^,],%16[^,]",value);
        track[i][4] = strtod(value,NULL);
        memset(value, 0x00, 16);

        sscanf(temp,"%*[^,],%*[^,],%*[^,],%*[^,],%*[^,],%16[^,]",value);
        track[i][5] = strtod(value,NULL);
        memset(value, 0x00, 16);

        sscanf(temp,"%*[^,],%*[^,],%*[^,],%*[^,],%*[^,],%*[^,],%16s",value);
        track[i][6] = strtod(value,NULL);
        memset(value, 0x00, 16);
*/

        memset(temp, 0x00, 32);
        if(fsize - ftell(fp) < 20)
            break;
        //        if (feof(fp) != 0)
        //            break;
        WGS84ToGuass((pTrack+i)->lat,(pTrack+i)->lon,&x,&y);
        //(pTrack+i)->lat = y;
        //(pTrack+i)->lon = x;
        (pTrack+i)->y = y;
        (pTrack+i)->x = x;

     //   printf("%f\n",(pTrack+i)->x);

    }
    _ker_printf("gps track is ready!\n");
//    _ker_printf("track1:%f\n",track[0][1]);
//    _ker_printf("track2:%f\n",track[0][2]);

    total = i;
    fclose(fp);
    return 1;
}

