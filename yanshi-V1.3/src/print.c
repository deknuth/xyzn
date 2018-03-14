#include "../inc/core.h"
#define FIFO_NAME "/tmp/AP_FIFO"        // pipe name
#define TEN_MEG ( 1024*1024*10 )		// buffer value
int pipe_fd;

void _printf(const char *format,...)
{
#ifdef DEBUG
    time_t now;
    struct tm  *timenow;
    time(&now);
    timenow = localtime(&now);
    printf("OTHER[%02d:%02d:%02d]: ",timenow->tm_hour,timenow->tm_min,timenow->tm_sec);
    va_list args;
    va_start(args,format);
    vprintf(format,args);
    va_end(args);
#endif
}

void _hal_printf(const char *format,...)
{
#ifdef HAL_DEBUG
    time_t now;
    struct tm  *timenow;
    time(&now);
    timenow = localtime(&now);
    printf("HAL[%02d:%02d:%02d]: ",timenow->tm_hour,timenow->tm_min,timenow->tm_sec);
    va_list args;
    va_start(args,format);
    vprintf(format,args);
    va_end(args);
#endif
}

void _gps_printf(const char *format,...)
{
#ifdef GPS_DEBUG
    time_t now;
    struct tm  *timenow;
    time(&now);
    timenow = localtime(&now);
    printf("GPS[%02d:%02d:%02d]: ",timenow->tm_hour,timenow->tm_min,timenow->tm_sec);
    va_list args;
    va_start(args,format);
    vprintf(format,args);
    va_end(args);
#endif
}

void _mmw_printf(const char *format,...)
{
#ifdef MMW_DEBUG
    time_t now;
    struct tm  *timenow;
    time(&now);
    timenow = localtime(&now);
    printf("MMW[%02d:%02d:%02d]: ",timenow->tm_hour,timenow->tm_min,timenow->tm_sec);
    va_list args;
    va_start(args,format);
    vprintf(format,args);
    va_end(args);
#endif
}

void _ker_printf(const char *format,...)
{
#ifdef KERNEL_DEBUG
    time_t now;
    struct tm  *timenow;
    time(&now);
    timenow = localtime(&now);
    printf("KERNEL[%02d:%02d:%02d]: ",timenow->tm_hour,timenow->tm_min,timenow->tm_sec);
    va_list args;
    va_start(args,format);
    vprintf(format,args);
    va_end(args);
#endif
}

void _lid_printf(const char *format,...)        //lidar
{
#ifdef LIDAR_DEBUG
    time_t now;
    struct tm  *timenow;
    time(&now);
    timenow = localtime(&now);
    printf("LIDAR[%02d:%02d:%02d]: ",timenow->tm_hour,timenow->tm_min,timenow->tm_sec);
    va_list args;
    va_start(args,format);
    vprintf(format,args);
    va_end(args);
#endif
}

int CreatPipe(void)
{
    int res = 0;
    int open_mode = O_WRONLY;                 // set file permission
    if(access(FIFO_NAME, F_OK) == -1 )
    {
        res = mkfifo(FIFO_NAME, 0777);
        if(res != 0)
        {                                 //管道文件不可重名
            printf( "Could not create fifo %s ", FIFO_NAME );
            return 0;
        }
    }
    pipe_fd = open(FIFO_NAME, open_mode);
    if(pipe_fd == -1)
    {
        close(pipe_fd);
        return 0;
    }
    else
    {
        setvbuf(stdout, NULL, _IONBF, 0);		// disable buffer
        dup2(pipe_fd, STDOUT_FILENO);		// redirect
    }
    return 1;
}
