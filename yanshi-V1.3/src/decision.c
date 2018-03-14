#include "../inc/core.h"

//FILE* fps;
void kernel(void)
{
    char latlon[48] = {0};
    unsigned char state = 0;
    int angle = 0;
    unsigned char bit = 0;
    double x; //current x coordinate value
    double y; //current y corrdinate value
    double c; //current c value
    if(initiateTrack(track_path)==0)
        goto end;
    //   PSELF pSelf = (PSELF)malloc(sizeof(SELF));
    PCOOR pCoor = (PCOOR)malloc(sizeof(COOR));
    if(pGps->status==5 || pGps->status==0x0b)
    {
        FILE *fp = fopen(out_path,"w+");
        while(1)
        {
            if(pGps->status==5 || pGps->status==0x0b)
            {
                if(!bit)
                    lprintf(lfd, INFO, "GPS: RTK signal normal.");
                bit = 1;
                state = START;
                WGS84ToGuass(pGps->latitude,pGps->longitude, &x, &y);
                pGps->x = x;
                pGps->y = y;
                pCoor->cur_x = x;
                pCoor->cur_y = y;
                pCoor->cur_azimuth = pGps->azimuth;
                pCoor->speed = pGps->vn;
                pCoor->cur_index = 0;
                /*
                pSelf->x = x;
                pSelf->y = y;
                pSelf->speed = 8;
                pSelf->index = 0;
                pSelf->azimuth = pGps->azimuth;
                */
                //       followTrack(pSelf,pCoor);
                if(total-(pCoor->n_index) < 5)
                {
                    state = STOP;
                    lprintf(lfd, INFO, "KERNEL: autopilot end!");
                    break;
                }
                angle = (STEER_CENTER_VALUE - followTrack(pCoor));
                //_ker_printf("angle: %d\n",ccpParam->angle);
                snprintf(latlon,47,"%f %f\n",x,y);
                fputs(latlon,fp);
                memset(latlon,0x00,48);
                usleep(20000);	// 延时20ms
            }
            else
            {
                if(bit)
                {
                    lprintf(lfd, WARN, "GPS: RTK signal abnormal!");
                    bit = 0;
                }
                state = PAUSE;
            }
            if(obstacle)
                state = PAUSE;
            VehCtrl(state,angle);
        }
        if(state != STOP)
        {
            if(g_state == PAUSE)
                state = PAUSE;
        }
        VehCtrl(state,angle);
        fclose(fp);
    }
    else
        lprintf(lfd, FATAL, "GPS: RTK signal abnormal!");
    free(pCoor);
end:
    sleep(2);
}
