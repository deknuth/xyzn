#include "../inc/core.h"
int RtkSet(char *path)
{
    int fd;
    char buffer[128] = {"$GPGGA,010753.000,2813.9798,N,11255.8564,E,1,6,2.31,166.6,M,-19.4,M,,*76"};
    fd = GPSComInit(path);
    if(fd == -1)
        return 0;
    else
    {
         _write(fd, buffer, strlen(buffer));
         close(fd);
    }
    return 1;
}
