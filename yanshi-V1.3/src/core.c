#include "../inc/core.h"
//char temp[48] = {"/home/lovenix/data/out_%s.txt"};
log_t *lfd;
char track_path[48] = {0};
char out_path[48] = {0};
int gfd;        // GPS com handle
sem_t sem_cs;
tpool_t *pool;
void quit(int a)
{
    printf("out: %d !\n",a);
    close(gfd);
    exit(0);
}

int main(int argc, char ** argv)
{
    if(argc != 3)
    {
        printf("Usage: ./auto track/track.txt out/out.txt\n");
        exit(0);
    }
    else
    {
        memcpy(track_path,argv[1],strlen(argv[1]));
        //sprintf(out_path,temp,argv[2]);
        memcpy(out_path,argv[2],strlen(argv[2]));
 //       printf("%s\n",track_path);
 //       printf("%s\n",out_path);
    }
    pthread_t id1,id2,id3,id4,id5,id6,id7;

    sem_init(&sem_cs, 0, 0);

    if((lfd = log_open(0)) == 0)
    {
        printf("Kernel: Open log file failed!");
        exit(0);
    }

    signal(SIGINT, quit);		// ctrl+C
#ifdef  PRINT
    if(CreatPipe() == 0)
    {
        _hal_printf("Pipe create falied!\n");
        exit(0);
    }
#endif
    /********** RTK PART **********/
    if(!RtkSet("/dev/ttyUSB1"))
        printf("RTK setting failed!\n");
    sleep(2);
#if 1
    /********** CAN PART **********/
    int i = 0;
    if(InitCAN(VCI_USBCAN2) == 0)		// Initialization device
        exit(0);
    GetCANInfo(VCI_USBCAN2);			// get device information
    pthread_create(&id1,NULL,(void *)ReceiveFrame,0);
    frame = (PVCI_CAN_OBJ)malloc(sizeof(VCI_CAN_OBJ));
    ccpParam = (PAP_PARAM)malloc(sizeof(AP_PARAM));
    memset(frame,0x00,sizeof(VCI_CAN_OBJ));
    memset(ccpParam,0x00,sizeof(AP_PARAM));
    InitFrame(frame);
    frame->Data[0] = REQ_CONTROL;		//request control

    for(i=0; i<10; i++)
    {
        if(ctrl_flag)
        {
            ctrl_flag = 0;
            break;
        }
        SendCTLFrame(frame);
        usleep(100000);	// 延时100ms
    }
    if(i < 10)
    {
        ControlInit();
        usleep(10000);	// delay 10ms
        if(pthread_create(&id2,NULL,(void *)ControlFrame,(void *)0) != 0)
            _hal_printf("pthread_create ControlFrame falied!\n");
    }
    else
    {
        _hal_printf("Get control fault!\n");
        CloseCAN(frame);
        goto end;
    }
    /*
    while(1)
    {
        _printf("input gear value(0-2):");
        scanf("%hhu",&(pParam->gear));
        _printf("input throttle value(0-4095):");
        scanf("%d",&(pParam->throttle));
        _printf("input angle value(400-1600):");
        scanf("%d",&(pParam->angle));
        _printf("input brake value(0-255):");
        scanf("%hhu",&(pParam->brake));
        _printf("\n");
        _printf("%d %d %d %d\n",pParam->gear,pParam->throttle,pParam->angle,pParam->brake);
    }
    */

    /************************** GPS PART ************************/
    pGps = (PGPS)malloc(sizeof(GPSINFO));
    if((gfd = GPSComInit("/dev/ttyUSB0")) == -1)
    {
        lprintf(lfd,FATAL,"GPS port open failure!");
        goto end;
    }
    if(pthread_create(&id3,NULL,(void *)GpsProcess,(void *)0) != 0)
        lprintf(lfd,FATAL,"GPS thread creation failure!");
    sleep(2);

    /********************** decision PART ***********************/
    if(pthread_create(&id4,NULL,(void *)kernel,0) != 0)
        lprintf(lfd,FATAL,"Decision thread creation failure!");

    /********************** lidar PART ***********************/
    if(pthread_create(&id5,NULL,(void *)LidarClient,(void *)0) != 0)
        lprintf(lfd,FATAL,"Lidar client thread creation failure!");
    usleep(2000);
    if(pthread_create(&id6,NULL,(void *)LidarCluster,(void *)0) != 0)
        lprintf(lfd,FATAL,"Lidar cluster thread creation failure!");
#endif
    /********************** socket PART ***********************/
    pool = tpool_init(128, 128, 1);

    if(pthread_create(&id7,NULL,(void *)TcpServer,(void *)TCP_PORT) != 0)
        lprintf(lfd,FATAL,"Socket server thread creation failure!");
#if 0
    pthread_join(id1,NULL);
    pthread_join(id2,NULL);
    pthread_join(id3,NULL);
    pthread_join(id4,NULL);
    pthread_join(id5,NULL);
    pthread_join(id6,NULL);
#endif
    pthread_join(id7,NULL);
end:
    CloseCAN(frame);
    sem_destroy(&sem_cs);
    tpool_destroy(pool, 1);
    free(pGps);
    free(pTrack);
    free(frame);
    free(ccpParam);
    log_close(lfd);
    return 1;
}
