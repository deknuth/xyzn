#ifndef HAL_H
#define HAL_H
#include "core.h"
#define STOP     0
#define START    1
#define PAUSE    2
typedef struct ap{
    unsigned int throttle;
    unsigned char brake;
    int angle;
    unsigned char gear;
    unsigned char state;        // 0->stop  1->start 2->pause
}AP_PARAM,*PAP_PARAM;			// 自动驾驶控制参数

extern PAP_PARAM ccpParam;        //chassis control struct pointer
extern unsigned char ctrl_flag;
extern PVCI_CAN_OBJ frame;
extern int InitCAN(unsigned int DevType);
extern int GetCANInfo(unsigned int DevType);			//获取CAN设备信息
extern int InitCAN(unsigned int DevType);
extern void ReceiveFrame(void);
extern void ControlFrame(void);
extern int InitFrame(PVCI_CAN_OBJ frame);
extern int SendCTLFrame(PVCI_CAN_OBJ frame);
extern void CloseCAN(PVCI_CAN_OBJ frame);
extern void ControlInit(void);
#define STEER_CENTER_VALUE  980
#define WHEELBASE   3.26
#define REQ_CONTROL 0x20        // request control
#endif

