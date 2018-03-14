#include "../inc/core.h"

#define S_FLAG	0
#define R_FLAG	1

static union {   
    char c[4];
    unsigned long l;
}endian_test;
#define ENDIANNESS ((char)endian_test.l)  

int count = 0;
unsigned char ctrl_flag = 0;
PVCI_CAN_OBJ frame;
//static int SendCTLFrame(PVCI_CAN_OBJ frame);
PAP_PARAM ccpParam;

int PrintFrame(PVCI_CAN_OBJ frame,unsigned char flag)
{
    int i = 0;
    if(flag)
        _hal_printf("RX ID:0x%08X\n",frame->ID);
    else
        _hal_printf("TX ID:0x%08X\n",frame->ID);
    /*
    if(frame->ExternFlag==0) _hal_printf(" Standard ");
    if(frame->ExternFlag==1) _hal_printf(" Extend   ");
    if(frame->RemoteFlag==0) _hal_printf(" Data   ");
    if(frame->RemoteFlag==1) _hal_printf(" Remote ");
    _hal_printf("DLC:0x%02X  ",frame->DataLen);

    _hal_printf("  data:");
    for(i=0;i<(frame->DataLen);i++)
        _hal_printf("%02X ",frame->Data[i]);
    _hal_printf("\n");
    */
    return 1;
}

void ControlFrame(void)
{
    unsigned int angle = 0;
    frame->Data[0] = 0x40;
    frame->Data[1] = 0x03;		// 自动驾驶模式

    //	frame->Data[2] = 0x1F;		// 前进档  h4bit
    //	frame->Data[3] = 0x80;		// 油门L8bit
    //	frame->Data[4] = 0x04;		// 方向H4Bit
    //	frame->Data[5] = 0x80;		// 方向L8Bit
    //	frame->Data[6] = 0x10;		// 刹车

    while(1)
    {
        frame->Data[6] = ccpParam->brake;		// 刹车
        angle = (ccpParam->angle)>1580?1580:ccpParam->angle;
        angle = (ccpParam->angle)<420?420:ccpParam->angle;
        frame->Data[4] = ((angle>>8) & 0xF);	// 方向H4Bit
        frame->Data[5] = (angle & 0xFF);		// 方向L8Bit
        frame->Data[3] = (ccpParam->throttle & 0xFF);	// 油门L8bit
        frame->Data[2] = ((ccpParam->throttle>>8) & 0xF);	// 油门H4bit
        if(ccpParam->gear < 3)		// 0->N  1->D  2->R
            frame->Data[2] |= ((ccpParam->gear)<<4);
        SendCTLFrame(frame);
        usleep(100000);	// 延时100ms
        /*
        frame->Data[3] += 1;
        if(frame->Data[3] > 254)
            frame->Data[3] = 0;
        */
        //  _hal_printf("Control frame!\n");
    }
}

void ReceiveFrame(void)
{
    int rLen = 0;
    unsigned int angle = 0;		// 转向角度
    int j;
    VCI_CAN_OBJ rec[3000];//接收缓存，设为3000为佳
    if('b' == ENDIANNESS)		//big endian
        _hal_printf("Big endian\n");
    else
        _hal_printf("Little endian\n");
    struct bs{
        unsigned char type : 2;
        unsigned char gear : 2;
        unsigned char mode : 1;
        unsigned char code : 3;
    }state;
    while(1)
    {
        if((rLen = VCI_Receive(VCI_USBCAN2,0,0,rec,3000,100))>0)//调用接收函数，如果有数据，进行数据处理显示。
        {
            for(j=0; j<rLen; j++)
            {
                //_hal_printf("Index:%04d  ",count);count++;//序号递增
                PrintFrame(&rec[j],R_FLAG);
                _hal_printf("TimeStamp:0x%08X\n",rec[j].TimeStamp);		// 时间标识。
                if(rec[j].ID == 0x82)		// back frame
                {
                    state.code = (rec[j].Data[0] & 0xE0) >> 5;
                    state.mode = (rec[j].Data[0] & 0x10) >> 4;
                    state.gear = (rec[j].Data[0] & 0x0C) >> 2;
                    state.type = (rec[j].Data[0] & 0x03);
                    angle = rec[j].Data[6]+((rec[j].Data[5]) << 8);
                    output = angle;
                    _hal_printf("Current steering angle: %d\n",angle);
                    switch(state.code)
                    {
                    case 1:
                        ctrl_flag = 1;
                        _hal_printf("Control allowable!\n");
                        break;
                    case 2:
                        _hal_printf("Control confirmation!\n");
                        break;
                    case 3:
                        _hal_printf("Data return!\n");
                        break;
                    case 4:
                        _hal_printf("End control confirmation!\n");
                        break;
                    case 5:
                        _hal_printf("Request denied!\n");
                        break;
                    default:
                        _hal_printf("Not defined!\n");
                        break;
                    }
                    if(state.mode)
                        _hal_printf("Automatic mode!\n");
                    else
                        _hal_printf("Manual mode!\n");
                    switch(state.gear)
                    {
                    case 0:
                        _hal_printf("Neutral gears!\n");
                        break;
                    case 1:
                        _hal_printf("Forward gears!\n");
                        break;
                    case 2:
                        _hal_printf("Reverse gears!\n");
                        break;
                    default:
                        break;
                    }
                    switch(state.type)
                    {
                    case 0:
                        _hal_printf("Vehicle normal!\n");
                        break;
                    case 1:
                        _hal_printf("Vehicle fault!\n");
                        break;
                    case 2:
                        _hal_printf("ECU fault!\n");
                        break;
                    case 3:
                        _hal_printf("ESTOP!\n");
                        break;
                    default:
                        break;
                    }
                }
            }
        }
    }
}

int InitFrame(PVCI_CAN_OBJ frame)	// 需要发送的帧，结构体设置
{
    int i = 0;
    frame->ID=0x81;
    frame->SendType=0;
    frame->RemoteFlag=0;
    frame->ExternFlag=0;
    frame->DataLen=8;
    for(i=0; i<(frame->DataLen); i++)
        frame->Data[i] = 0;
    return 1;
}

int GetCANInfo(unsigned int DevType)			//获取CAN设备信息
{
    int i = 0;
    VCI_BOARD_INFO pInfo;
    if(VCI_ReadBoardInfo(DevType,0,&pInfo)==1)//读取设备序列号、版本等信息。
    {
        _hal_printf("Get device info success!\n");
        /*
        _hal_printf("Serial Number:");
        for(i=0; i<20; i++)
            _hal_printf("%c", pInfo.str_Serial_Num[i]);
        _hal_printf("\n");

        _hal_printf("HW Type:");
        for(i=0; i<10; i++)
            _hal_printf("%c", pInfo.str_hw_Type[i]);
        _hal_printf("\n");
        */
        return 1;
    }
    else
    {
        _hal_printf("Get can info error!\n");
        return 0;
    }
}

int SendCTLFrame(PVCI_CAN_OBJ frame)
{
    if(VCI_Transmit(VCI_USBCAN2, 0, 0, frame, 1) == 1)
    {
        PrintFrame(frame,S_FLAG);
        return 1;
    }
    else
        return 0;
}

int InitCAN(unsigned int DevType)
{
    if(VCI_OpenDevice(VCI_USBCAN2,0,0) == 1)	//打开设备
        _hal_printf("Open CAN1 success!\n");
    else
    {
        _hal_printf("Open CAN1 error!\n");
         lprintf(lfd,FATAL,"CAN: Open CAN1 error!");
        return 0;
    }
    //初始化参数，严格参数二次开发函数库说明书
    VCI_INIT_CONFIG config;
    config.AccCode=0x80000000;
    config.AccMask=0xFFFFFFFF;
    config.Filter=1;        //接收所有帧
    config.Timing0=0x00;    //波特率125 Kbps  0x03  0x1C
    config.Timing1=0x1C;
    config.Mode=0;          //正常模式

    if(VCI_InitCAN(DevType,0,0,&config)!=1)
    {
        _hal_printf("Init CAN1 error!\n");
        lprintf(lfd,FATAL,"CAN: Init CAN1 error!");
        VCI_CloseDevice(DevType,0);
        return 0;
    }
    if(VCI_StartCAN(DevType,0,0)!=1)
    {
        _hal_printf("Start CAN1 error!\n");
        lprintf(lfd,FATAL,"CAN: Start CAN1 error!");
        VCI_CloseDevice(DevType,0);
        return 0;
    }
    return 1;
}

void ControlInit(void)
{
    ccpParam->angle = STEER_CENTER_VALUE;		// 方向居中
    ccpParam->throttle = 0;		// 油门归零
    ccpParam->brake = 0;        // 刹车归零
    ccpParam->gear = 0;         // 空挡
}

void CloseCAN(PVCI_CAN_OBJ frame)
{
    VCI_ResetCAN(VCI_USBCAN2, 0, 0);	// 复位CAN1通道
    usleep(100000);// delay 100ms
    VCI_ResetCAN(VCI_USBCAN2, 0, 1);
    usleep(100000);
    VCI_CloseDevice(VCI_USBCAN2,0);		// 关闭设备
    free(frame);
    free(ccpParam);
}


