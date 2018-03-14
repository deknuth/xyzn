#ifndef CORE_H
#define CORE_H
#include 	<stdio.h>
#include 	<stdlib.h>
#include 	<string.h>
#include 	<stdarg.h>
#include 	<errno.h>
#include 	<math.h>
#include 	<sys/types.h>
#include 	<netinet/in.h>
#include 	<sys/socket.h>
#include 	<sys/wait.h>
#include 	<unistd.h>
#include 	<arpa/inet.h>
#include 	<sys/mman.h>
#include 	<syscall.h>
#include 	<fcntl.h>
#include 	<sys/epoll.h>
#include 	<sys/time.h>
#include 	<sys/resource.h>
#include 	<pthread.h>
#include 	<sys/stat.h>
#include 	<locale.h>
#include 	<semaphore.h>
#include 	<signal.h>
#include 	<setjmp.h>
#include 	<time.h>
#include 	<limits.h>
#include 	<ctype.h>
#include 	<float.h>
#include 	<stddef.h>
#include	<netdb.h>
#include 	<assert.h>
#include	<termios.h>
#include    "public.h"
#include    "gps.h"
#include    "decision.h"
#include    "kmp.h"
#include    "print.h"
#include    "can.h"
#include    "coordinates.h"
#include    "hal.h"
#include    "follow.h"
#include    "control.h"
#include    "lidar.h"
#include    "log.h"
#include    "socket.h"
#include    "pool.h"
#include    "crc.h"
extern int gfd;
extern sem_t sem_cs;        // control signal
extern FILE* fps;
extern char track_path[];
extern char out_path[];
extern int RtkSet(char *path);
#endif
