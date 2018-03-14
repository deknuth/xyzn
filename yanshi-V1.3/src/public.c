#include "../inc/core.h"

double delta_x(double angle)
{
    double coeff_lane_x = 0;
    return (coeff_lane_x*cos(angle * IPI));
}

double delta_y(double angle)
{
    double coeff_lane_y = 0;            // wide of road
    return (coeff_lane_y*sin(angle * IPI));
}

double distOfPoint(double x1, double y1, double x2, double y2)
{
    return sqrt(((x2-x1)*(x2-x1) + (y2-y1)*(y2-y1)));
}

//azimuth constructed by current and aim point
double cacuAimCurAzimuth(PCOOR pCoor)
{
    double result,alfa;
    pCoor->aim_x = (pTrack+pCoor->aim_index)->x + delta_x(pCoor->aim_azimuth);
    pCoor->aim_y = (pTrack+pCoor->aim_index)->y + delta_y(pCoor->aim_azimuth);

    result  = (pCoor->aim_y-pCoor->cur_y) * 1 / sqrt((pCoor->aim_x-pCoor->cur_x) * (pCoor->aim_x-pCoor->cur_x) + (pCoor->aim_y-pCoor->cur_y) * (pCoor->aim_y-pCoor->cur_y) );
    alfa  = acos(result) * 180 / 3.1411592627;
    _ker_printf("alfa:%.8lf\n",alfa);
    if(pCoor->aim_x >= pCoor->cur_x) //right side
        ;
    else
        alfa = 360 - alfa;
    return alfa;
}

/*cur_azimuth is amimuth of the vehicle in current status.
  aim_azimuth is amimuth of aim point  */
double delta_azimuth(double cur_azimuth, double aim_azimuth)
{
    double delta_angle;
    delta_angle = cur_azimuth - aim_azimuth; /*>0, steering wheel turns left hand, counter-clockwise; < 0, turn right hand, clockwise*/
    if(delta_angle >180)
        delta_angle = delta_angle - 360;
    else if(delta_angle < -180)
        delta_angle = delta_angle + 360;
    else{; }//do nothing

    return delta_angle;
}

double verticalLineDist(PCOOR pCoor)
{
    /*
    double start_x = 0, start_y = 0;
    double end_x = 0, end_y = 0;
    double delta_dist = 0;
    double k = 0, b = 0;   // y = kx + b;
//    printf("cur:%d\taim:%d\n",pCoor->cur_index,pCoor->aim_index);

    start_x = track[pCoor->cur_index][1] + delta_x(pCoor->cur_index); //longitude 388011.84;//
    start_y = track[pCoor->cur_index][2] + delta_y(pCoor->cur_index); //latitude 3123060.50;//

    end_x = track[pCoor->aim_index][1] + delta_x(pCoor->aim_index);
    end_y = track[pCoor->aim_index][2] + delta_y(pCoor->aim_index);

 //    printf("%lf\t%lf\t%lf\t%lf\n",start_x,end_x,start_y,end_y);

    if(end_x == start_x)
        delta_dist = fabs(pCoor->cur_x - start_x);
    else
    {
        k = (end_y - start_y) / (end_x - start_x);
        b = start_y - k * start_x; //b = y1- k*x1
        delta_dist = fabs(k * pCoor->cur_x - pCoor->cur_y + b) / sqrt(k*k + 1*1) ;//kx-y+b = 0; (Ax0+By0+c)/sqrt(A^2+B^2)
    }
    _ker_printf("k: %lf\tb: %lf\n",k,b);
    if((int)includedAngle(pCoor) < 0)  // Judge the direction
        delta_dist = 0 - delta_dist;    //the distance has the + or -.
    return delta_dist;
    */
    return 0;
}


/*caculated the included angle for judge the vehicle in which side*/
/*>0, steering wheel turns left hand, counter-clockwise; < 0, turn right hand, clockwise*/

double includedAngle(PCOOR pCoor)
{
    double k = 0;
    unsigned int  start_x,start_y;
    unsigned int  end_x,end_y;
    double delta_angle_aim,delta_angle_cur;
    start_x = (pTrack+pCoor->cur_index)->x + delta_x(pCoor->cur_index); //longitude 388011.84;//
    start_y = (pTrack+pCoor->cur_index)->y + delta_y(pCoor->cur_index);

    end_x = (pTrack+pCoor->aim_index)->x + delta_x(pCoor->aim_index);
    end_y = (pTrack+pCoor->aim_index)->y + delta_y(pCoor->aim_index);

    //y = kx + b
    /*caculate the aim included angle*/
    if(end_x == start_x)
    {
        if(end_y > start_y) //upward
            delta_angle_aim = 90;
        else if(end_y < start_y) //downward
            delta_angle_aim = -90;
        else
            delta_angle_aim = 0;
    }
    else
    {
        k = (end_y - start_y) / (end_x - start_x);
        delta_angle_aim = atan(k) * 180 / 3.141592627;
    }

    /*caculate the cur included  angle*/
    if(pCoor->cur_x == start_x)
    {
        if(pCoor->cur_y > start_y) //upward
            delta_angle_cur = 90;
        else if(pCoor->cur_y < start_y) //downward
            delta_angle_cur = -90;
        else
            delta_angle_cur = 0;
    }
    else
    {
        k = (pCoor->cur_y - start_y) / (pCoor->cur_x - start_x);
        delta_angle_cur = atan(k) * 180 / 3.141592627;
    }

    /*>0, steering wheel turns left hand, counter-clockwise; < 0, turn right hand, clockwise*/
    return  delta_angle_aim - delta_angle_cur;
}

unsigned long GetFSize(const char *path)        // get file size
{
    unsigned long filesize = -1;
    struct stat statbuff;
    if(stat(path, &statbuff) < 0)
        return filesize;
    else
        filesize = statbuff.st_size;
    return filesize;
}

int _write(int fd, void *buffer, int length)
{
    int bytes_left;
    int written_bytes;
    char *ptr;

    ptr = buffer;
    bytes_left = length;
    while (bytes_left > 0)
    {
        /********开始写 ***********/
        written_bytes=write(fd, ptr, bytes_left);
        if (written_bytes <= 0) // 出错了
        {
            if (errno==EINTR) // 中断错误继续写
                written_bytes = 0;
            else
                // 其他错误退出
                return (-1);
        }
        bytes_left -= written_bytes;
        ptr += written_bytes; // 从剩下的地方继续写
    }
    return 0;
}

void StringToHex(unsigned char *dst,const char *src)	// ACII to hex 	only 0-F
{
    int i,j;
    int sLen = strlen(src);
    const unsigned ASCValue[128] = {
            0,0,0,0,0,0,0,0,0,0,		// 0-9
            0,0,0,0,0,0,0,0,0,0,
            0,0,0,0,0,0,0,0,0,0,
            0,0,0,0,0,0,0,0,0,0,
            0,0,0,0,0,0,0,0,0,1,
            2,3,4,5,6,7,8,9,0,0,
            0,0,0,0,0,10,11,12,13,14,
            15,0,0,0,0,0,0,0,0,0,
            0,0,0,0,0,0,0,0,0,0,
            0,0,0,0,0,0,0,10,11,12,
            13,14,15,0,0,0,0,0,0,0,
            0,0,0,0,0,0,0,0,0,0,
            0,0,0,0,0,0,0,0
    };
    for(i = 0,j = 0; i < sLen;)
    {
        dst[j] = ASCValue[(unsigned char)src[i++]];
        dst[j] <<= 4;
        dst[j++] += ASCValue[(unsigned char)src[i++]];
    }
}

int HexToString(char *dst,const unsigned char *src,int sLen)	// hex to ASCII
{
    const char* hexDigits = "0123456789ABCDEF";
    int a,b,j = 0,k = 0;
    char c;
    for(k = 0; k < sLen; k++)
    {
        c = src[k];
        if((c & 0x80) > 0)		// c > 128
        {
            a = c & 0x0F;
            b = c & 0xF0;
            dst[j++] = hexDigits[(a+b)/16];
            dst[j++] = hexDigits[a%16];
        }
        else
        {
            dst[j++] = hexDigits[c/16];
            dst[j++] = hexDigits[c%16];
        }
    }
    dst[j] = '\0';
    return j;
}
