#ifndef SYSVAR_H
#define SYSVAR_H

#include <QString>
#include <QTimer>

#define VERSION  "VERSION 1.0.0:2021-02-06 14:50:03"
#define DEBUG_ON_PC
//#define DEBUG
#ifdef DEBUG
#define DEBUGPRINTF qDebug
#else
#define DEBUGPRINTF(...)
#endif
#define ENBALE_ALSA
/*
 * 服务端数据
 */
#define MAXTCPCLIENTS 1
#define MAXTCPBUFSIZE (1024 * 1024)
#define SOCKETTIMEOUT 3000   //3000ms
#define SERVERPORT 5555
/*
 * 客户端数据
 */
#define HOSTIP   "192.168.1.31"
#define HOSTPORT 15000

/* 二进制数据传输类型标示 */
#define LOCAL_MIC_DATA_TYPE         0X01
#define REMOTE_RADIO_DATA_TYPE      0X71
#define REMOTE_RADIO_COMMANDTO_DEVICE_TO_PC__TYPE   0X02
#define REMOTE_RADIO_COMMANDTO_PC_TO_DEVICE__TYPE   0X03
#define REMOTE_RADIO_DATA_PC_TO_DEVICE_PLAY         0X04//优先级最低
#define HEAR_BEAT                                   0XEF

enum ConnecType{
  TCP = 1,
  UDP = 2,
};
/*
 * LAN定义
*/
struct lanDef
{
  char ip[32];                   //IP地址
  char hostIp[64];
  char mask[32];                 //子网掩码
  char gate[32];                 //网关
  char mac[32];                  //MAC地址
  int  hostPort;
  unsigned int port;             //端口
};

struct s_rmtp{
  QString isEnable;
  QString HostIp;
  quint16 HostPort;
  quint16 HostPort4Bin;
  quint16 HostPort4Sound;
  int interval;
  bool enable;
};
//CheckStatus,Power=OK,Net=On,Audio=Off,RadioConnect=OK,RadioType=111
struct Status{
    QTimer *collectTimeState;
    QString power;
    QString net;
    QString audio;
    QString radioConnect;
    QString radioType;
    int collectTimeStep;/* 采集电台工作信息间隔时间 */
    int stausTimeStep;//设备状态上报间隔时间等参数设置
    QString allString;
};
/* radio & mic 使用的串口情况 */
enum readioMic{
    READIO_MIC_OK,
    READIO_MIC_ERR,
};
typedef struct{
    enum readioMic readio;
    enum readioMic mic;
}ReadioMicState_t;
/** 发送和接收的内容 **/
typedef struct{
    unsigned char head;
    unsigned short length;
    char guid[16];
    unsigned char dataType;
    unsigned char data[4410];
    unsigned char end;
}dataFormat_t;

/* 采集板的自检/状态上报,CheckStatus,GUID=…,Power=OK,Net=On,Audio=Off,RadioConnect=OK,RadioType=111 */
typedef struct{

    QString power;
    QString net;
    QString audio;
    QString radioConnect;
    QString radioType;
    QString allString;
}collectState_t;
enum WorkMode{
    SELECT_RADIO_MODE,
    SELECT_MIC_MODE,
};
typedef struct {
    QString uartRadio;
    int uartRadioBaud;
    QString uartMic;
    int uartMicBaud;
}UartSelect_t;

typedef struct{
    unsigned int direct_sellect_state;
    unsigned int radio_state;
    unsigned int data_cable_state;

} communicate_state_t;
//系统数据结构
struct SysData
{
    lanDef lan;
    ConnecType connect_type;
    QString guidRoot;
    QString guidClient;
    char guidClienthex[16];
    struct s_rmtp gt_rmtp;
    struct Status checkStatus;//
    bool collect_radio_data_enable;//询问电台设备的工作状态
    ReadioMicState_t readioMicState;
    enum WorkMode workMode;
    UartSelect_t uartSelect;
    communicate_state_t communicate_state;
};


extern SysData sysData;
#endif // SYSVAR_H
