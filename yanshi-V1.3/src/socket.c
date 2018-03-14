#include "../inc/core.h"

unsigned char g_state = START;
static int setnonblocking(int sockfd);
static void TcpProcess(int tfd);
static void TcpSend(int tfd);
static int ProtoAnalys(char *buf,int len);

/******* setnonblocking - 设置句柄为非阻塞方式 *******/
int setnonblocking(int sockfd)
{;
    struct timeval timeout = { 3, 0 };
    int mw_optval = 1;
    int nRecvBuf = 512*1024;		//	512K
    int nSendBuf = 512*1024;		//	512K
    setsockopt(sockfd,SOL_SOCKET,SO_REUSEPORT,(char *)&mw_optval,sizeof(mw_optval));
    setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,(char *)&mw_optval,sizeof(mw_optval)); 	// 设置端口多重邦定
    setsockopt(sockfd,SOL_SOCKET,SO_SNDTIMEO,(char *)&timeout,sizeof(timeout));
    setsockopt(sockfd,SOL_SOCKET,SO_RCVTIMEO,(char *)&timeout,sizeof(timeout));
    setsockopt(sockfd,SOL_SOCKET,SO_RCVBUF,(const char*)&nRecvBuf,sizeof(int));
    setsockopt(sockfd,SOL_SOCKET,SO_SNDBUF,(const char*)&nSendBuf,sizeof(int));
    if (fcntl(sockfd, F_SETFL, fcntl(sockfd, F_GETFD, 0)|O_NONBLOCK) == -1)
        return 0;
    return 1;
}

void *TcpServer(void *arg)
{
    int tfd,socket_fd;
    fd_set fdr;
    struct timeval tv;
    struct sockaddr_in ser_addr;
    //    struct in_addr addr;
    //    inet_aton(SERVER_IP,&addr);
    socklen_t len = sizeof(ser_addr);

    //    tpool_add_work(pool, timer, 150);

    if ((tfd = socket(PF_INET, SOCK_STREAM, 0)) == -1) 	// 开启 socket 监听
    {
        lprintf(lfd, FATAL, "TCP: TCP Socket error!");
        return 0;
    }
    else
        lprintf(lfd, INFO, "TCP: TCP socket create success!");

    setnonblocking(tfd);		// 设置非阻塞模式

    bzero(&ser_addr, sizeof(ser_addr));
    ser_addr.sin_family = PF_INET;
    ser_addr.sin_port = htons((uint16_t) atoi(arg));
    ser_addr.sin_addr.s_addr = INADDR_ANY;

    if(bind(tfd, (struct sockaddr *) &ser_addr, sizeof(struct sockaddr)) == -1)
    {
        lprintf(lfd, FATAL, "TCP: TCP bind error!");
        return 0;
    }
    else
        lprintf(lfd, INFO, "TCP: TCP bind success!");
    if (listen(tfd, 20) == -1)
    {
        lprintf(lfd, FATAL, "TCP: TCP listen error!");
        return 0;
    }
    else
        lprintf(lfd, INFO, "TCP: TCP listen success!");
    while (1)
    {
        FD_ZERO(&fdr);
        FD_SET(tfd, &fdr);
        tv.tv_sec = 5;
        switch (select(tfd+1, &fdr, NULL, NULL, &tv))
        {
        case -1:
            break;
        case 0:
            break;
        default:
            if (FD_ISSET(tfd, &fdr))
            {
                if((socket_fd = accept(tfd, (struct sockaddr *) &ser_addr, &len)) > 0)
                {
                    lprintf(lfd, INFO, "TCP: Accept from: %s:%d\n",inet_ntoa(ser_addr.sin_addr), ntohs(ser_addr.sin_port));
                    tpool_add_work(pool, TcpProcess, socket_fd);
                    tpool_add_work(pool, TcpSend, socket_fd);
                }
            }
            break;
        }
    }
}

void TcpProcess(int tfd)
{
    int rLen = 0;
    char buf[1024] = {0};
    struct timeval tv;
    fd_set fdr;
    if (fcntl(tfd, F_SETFL, fcntl(tfd, F_GETFD, 0)|O_NONBLOCK) == -1)
        return;
    while(1)
    {
        FD_ZERO(&fdr);
        FD_SET(tfd, &fdr);
        tv.tv_sec = 2;
        switch (select(tfd+1, &fdr, NULL, NULL, &tv))
        {
        case -1:
            goto end;
        case 0:
            break;
        default:
            if (FD_ISSET(tfd, &fdr))
            {
                rLen = read(tfd, buf, 1024);
                if(rLen > 0)
                {
                    printf("%s\n",buf);
                    ProtoAnalys(buf,rLen);
                    memset(buf,0x00,rLen);
                }
                else if (rLen == -1 && errno != EAGAIN)			// socket closed
                    goto end;
                else if(rLen == 0)
                    goto end;
            }
            break;
        }
    }
end:
    close(tfd);		// close client
}

int ProtoAnalys(char *buf,int len)
{
    unsigned int crc = 0;
    char temp[16] = {0};
    char surplus[128] = {0};
    unsigned char pLen = 0;
    struct{
        char len[3];
        unsigned int crc;
        char type[16];
        char para1[32];
        char para2[32];
        char para3[32];
        char para4[32];
        char para5[32];
        unsigned char para_num;
    }sProt;
    memset(&sProt,0x00,sizeof(sProt));
    // #f06bed7b,19,GOTO,200.11,300.25
    if(*buf != '#' || len > 128)
        return 0;
    else
    {
        pLen = len-9;
        buf++;
        memcpy(temp,buf,8);
        buf += 8;
        sProt.crc = strtoul(temp,0,16);
        memcpy(surplus,buf,pLen);
        crc = crc32(surplus,pLen);
        printf("crc: %X,%X\n",crc,sProt.crc);
        if(crc == sProt.crc)
        {
            buf++;          // jump ,
            if(sscanf(buf,"%3[^,]",sProt.len) == 1)
            {
                if(sscanf(buf,"%*[^,],%16[^,]",sProt.type) == 1)
                {
                    if(sscanf(buf,"%*[^,],%*[^,],%32[^,]",sProt.para1) == 1)
                    {
                        sProt.para_num++;
                        if(sscanf(buf,"%*[^,],%*[^,],%*[^,],%32[^,]",sProt.para2) == 1)
                        {
                            sProt.para_num++;
                            if(sscanf(buf,"%*[^,],%*[^,],%*[^,],%*[^,],%32[^,]",sProt.para3) == 1)
                            {
                                sProt.para_num++;
                                if(sscanf(buf,"%*[^,],%*[^,],%*[^,],%*[^,],%*[^,],%32[^,]",sProt.para4) == 1)
                                    sProt.para_num++;

                            }
                            if(strcmp(sProt.type,"GOTO") == 0)
                            {
                                printf("coor: %s\t%s\n",sProt.para1,sProt.para2);
                            }
                            else if(strcmp(sProt.type,"CTRL") == 0)
                            {
                                if(strcmp(sProt.para1,"STOP") == 0)
                                    g_state = PAUSE;
                                else if(strcmp(sProt.para1,"START") == 0)
                                    g_state = START;
                            }
                        }
                        else
                            return 0;
                    }
                    else
                        return 0;
                }
                else
                    return 0;
            }
            else
                return 0;
        }
    }
    return 1;
}

void TcpSend(int tfd)
{
    int len = 0;
    int sLen = 0;
    unsigned int crc;
    char str1[64] = {0};
    char str2[64] = {0};
    char pack[128] = {0};

    while(1)
    {
        len = snprintf(str1,63,",POS,%.6lf,%.6lf",pGps->x,pGps->y);
        if(len > 0)
        {
            len = snprintf(str2,63,",%d%s",len,str1);
            if(len > 0)
            {
                crc = crc32(str2,len);
                len = snprintf(pack,127,"#%X%s",crc,str2);
            }
           // printf("%s\n",pack);
        }
        if(len > 0)
        {
            sLen = send(tfd, pack, len ,0);
            if (sLen == -1 && errno != EAGAIN)			// socket closed
                goto end;
            else if(sLen == 0)
                goto end;
        }
        memset(str1,0x00,64);
        memset(str2,0x00,64);
        memset(pack,0x00,128);
        usleep(200000);
    }
end:
    close(tfd);
}

