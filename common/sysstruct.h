/*
 * sysstruct.h
 *
 *  Created on: 2015-10-26
 *      Author: Administrator
 */

#ifndef SYSSTRUCT_H_
#define SYSSTRUCT_H_

#include <QString>
#include <stdio.h>

#define DEBUG_INFO
#ifdef DEBUG_INFO
#define __info(fmt,...)  do{printf("[%s],<%s>,(%d):",__FILE__,__FUNCTION__,__LINE__);\
                            printf(fmt,##__VA_ARGS__);\
    fflush(stdout);\
                            }while(0)
#else
#define __info
#endif


#define __DEBUG       1
#ifdef __DEBUG
#define DebugPrintf(fmt, args...);    do{printf(fmt, ##args); fflush(stdout);}while(0)
#else
#define DebugPrintf
#endif
#define __SAVELOGDEBUG 0

#define DEVICEID      0x2001  //connect for pc

#define VER_MAJOR     0
#define VER_MINOR     1
#define VER_REVISION  0
#define VER_BUILD     190111

#define INTSOCKETBUFSIZE 2048
#define INTSLEEPUS    0 * 1000 //10 ms

#define MAXTCPCLIENTS  1
#define MAXTCPBUFSIZE (1024 * 1024)
#define SOCKETTIMEOUT 3000   //3000ms

static unsigned char  SCPIEOFCHAR[]         = {0x0d, 0x0a};
static unsigned char  SCPIBINEOFCHAR[]      = {0x07, 0xd0};
static unsigned char  SCPICMDEOF[]          = {0x0d, 0x0a};
static unsigned char  SCPIRESULTEOF[]       = {0x0d, 0x0a};

enum measStateDef {msIdle, msStarted, msStarting, msStopped, msStopping, };

//SCPI接口定义
enum scpiInterfaceType {sitNone, sitLan, sitUsb, sitGpib, sitCom, sitRmtp};

typedef enum { COLLECT_RECEIVE, DISTURB, COLLECT_SEND, DISCONNECT} enum_sound_channel_select;

typedef enum { ENBALE_AMPLIFY, DISABLE_AMPLIFY} enum_sound_amplify;
typedef enum { ENBALE_PTT, DISABLE_PTT} enum_PTT_contrl;
//设备信息
struct devDef
{
  char company[128];             //公司
  char devType[64];              //设备类型
  char serialNoPrefix[64];       //序列号前缀
  char serialNo[32];             //序列号   BL XX XX XX XX XXX 1/2

};



//SCPI通信数据结构定义
struct scpiCommDataDef
{
  scpiInterfaceType interface;

  bool stateR;
  bool stateL;
  bool stateT;
  bool stateS;

  bool serialOpened;
  bool gpibOpened;
  bool lanOpened;
  bool usbOpened;

  unsigned int rcvLength;
};

//RMTP
struct rmtp{
  QString isEnable;
  QString HostIp;
  QString HostIpBackUp;
  int HostPort;
  int HostPortBackUp;
  int interval;
};

//系统数据结构
struct dataDef
{
  struct scpiCommDataDef scpiData;  //SCPI信息
  measStateDef measState;           //系统工作状态

};

#endif /* SYSSTRUCT_H_ */
