/*
 * SCPI指令设置文件
 */
#ifndef COMMAND_H_
#define COMMAND_H_
#include <QString>

#define MAX_GROUP                                    20
#define VALUE_MAX_GROUP                              32

//错误代码
#define SCPI_CMD_UNKNOWEN                            -1000  //未知

#define __SCPI_SUCCESS                                0     //SCPI成功
#define __SCPI_FAILED                                 -1    //失败
#define __SCPI_IGNORE                                 -2    //忽略
#define __SCPI_CANCEL                                 -3    //取消
#define __SCPI_UNSUPPORT                              -4    //不支持
#define __SCPI_RETURN                                 -5    //返回
#define __SCPI_NONE                                   -6    //无

//标准指令
#define SCPI_CMD_STANDARD_CLS                         0x1000
#define SCPI_CMD_STANDARD_ESE                         0x1001
#define SCPI_CMD_STANDARD_ESE_QUERY                   0x1002
#define SCPI_CMD_STANDARD_ESR_QUERY                   0x1003
#define SCPI_CMD_STANDARD_IDN_QUERY                   0x1004
#define SCPI_CMD_STANDARD_OPC                         0x1005
#define SCPI_CMD_STANDARD_OPC_QUERY                   0x1006
#define SCPI_CMD_STANDARD_RST                         0x1007
#define SCPI_CMD_STANDARD_SRE                         0x1008
#define SCPI_CMD_STANDARD_SRE_QUERY                   0x1009
#define SCPI_CMD_STANDARD_STB_QUERY                   0x100A
#define SCPI_CMD_STANDARD_TRG                         0x100B
#define SCPI_CMD_STANDARD_WAI                         0x100C
#define SCPI_CMD_STANDARD_CAL                         0x100D
#define SCPI_CMD_STANDARD_VER                         0x100E
#define SCPI_CMD_STANDARD_REG                         0x100F
#define SCPI_CMD_STANDARD_CALED_QUERY                 0x1010
#define SCPI_CMD_STANDARD_REBOOT                      0x1011
#define SCPI_CMD_STANDARD_HEART_BEAT                           0X1012

#define SCPI_CMD_DELETE_LOG_TEST                                        0xCFFF

/*TCP Client commond*/
#define RMTP_CMD_SERVER_SET_ENABLE                                      0XE001
#define RMTP_CMD_SERVER_SET_IP                                          0XE002
#define RMTP_CMD_SERVER_GET_IP                                          0XE003
#define RMTP_CMD_SERVER_SET_PORT                                        0XE004
#define RMTP_CMD_SERVER_GET_PORT                                        0XE005
#define RMTP_CMD_CONNECT_SET_INTERVAL                                   0XE006
#define RMTP_CMD_CONNECT_GET_INTERVAL                                   0XE007
#define RMTP_CMD_SENSOR_GET_INFORMATION                                 0xE008
#define RMTP_CMD_SENSOR_SAVE_IQDATA                                     0xE009
#define RMTP_CMD_SERVER_RECEIVE_IQDATA                                  0xE00A
#define RMTP_CMD_SERVER_RECEIVE_LATLOMTIM                               0xE00B
#define RMTP_CMD_SENSOR_TDOA_DEMOBAND                                   0xE00C
#define RMTP_CMD_SENSOR_TDOA_FILTER                                     0xE00D

#define RMTP_CMD_GET_SOUND_DATA                                         0x2000
#define RMTP_CMD_PLAYER_SOUND_DATA                                      0x2001
#define RMTP_CMD_COLLECT_RADIO_STATUS                                   0X2002
#define RMTP_CMD_SET_PARAMETER                                          0X2003

#define SYSTEM_SET_DATA_TIME                                            0x3000
#define SYSTEM_GET_DATA_TIME                                            0x3001
#define SYSTEM_HEART_BEAT                                               0x3002
#define SYSTEM_CHRECK_STATE                                             0x3003
#define SYSTEM_RESTART_APP                                              0X3004
#define SYSTEM_CHECK_STATUS                                             0X3005 
#define SYSTEM_COLLECT_RADIO_STATUS                                     0X3006
#define SYSTEM_RECORD_AUDIO_ENABLE                                      0X3007
#define SYSTEM_SET_GUID                                                 0X3008
#define SYSTEM_SET_AMPLIFY                                              0X3009
#define SYSTEM_SET_STATE_DISTURB                                        0X300A

struct _SCPI_keyboardCmd
{
  char data[128];
  unsigned long funcode;
};

static struct _SCPI_keyboardCmd Mainkey[]=
{
  {"*CLS",                                                          SCPI_CMD_STANDARD_CLS},\
  {"*IDN?",                                                         SCPI_CMD_STANDARD_IDN_QUERY},\
  {"*RST",                                                          SCPI_CMD_STANDARD_RST},\
  {"*FACTCAL",                                                      SCPI_CMD_STANDARD_CAL},\
  {"*VER?",                                                         SCPI_CMD_STANDARD_VER},\
  {"*TRG",                                                          SCPI_CMD_STANDARD_TRG},\
  {"*OPC?",                                                         SCPI_CMD_STANDARD_OPC_QUERY},\
  {"*CAL?",                                                         SCPI_CMD_STANDARD_CALED_QUERY},\
  {"*REB",                                                          SCPI_CMD_STANDARD_REBOOT},\
};

static struct _SCPI_keyboardCmd test[] = {
  //ROOT
    {"",                                                             0x00000000},\
    {":HEArt:BEAt",                                                  SCPI_CMD_STANDARD_HEART_BEAT},
    {":DELete",                                                      SCPI_CMD_DELETE_LOG_TEST},\

  //COMMAND LIST
  /*RMTP TCP Client commend*/
    {":RMTP:ENABle",                                                RMTP_CMD_SERVER_SET_ENABLE },\
    {":RMTP:SERVer:SET:IP",                                         RMTP_CMD_SERVER_SET_IP },\
    {":RMTP:SERVer:IP?",                                            RMTP_CMD_SERVER_GET_IP },\
    {":RMTP:SERVer:SET:PORT",                                       RMTP_CMD_SERVER_SET_PORT },\
    {":RMTP:SERVer:PORT?",                                          RMTP_CMD_SERVER_GET_PORT },\
    {":RMTP:CONNect:SET:INTErval",                                  RMTP_CMD_CONNECT_SET_INTERVAL },\
    {":RMTP:CONNect:GET:INTErval?",                                  RMTP_CMD_CONNECT_GET_INTERVAL },\
    {":RMTP:SENSor:INFOrmation?",                                   RMTP_CMD_SENSOR_GET_INFORMATION },\
    {":RMTP:CHECk:STATus",                                          SYSTEM_CHECK_STATUS},
    {":RMTP:SET:PARAmeter",                                         RMTP_CMD_SET_PARAMETER},

    /* get sound data & player sound data */
    {":RMTP:CMD:GET:SOUNd:DATA",                                    RMTP_CMD_GET_SOUND_DATA },\
    {":RMTP:PLAYer:SOUNd:DATA",                                     RMTP_CMD_PLAYER_SOUND_DATA },\
    {":RMTP:CMD:COLLect:RADIo:STATus?",                             RMTP_CMD_COLLECT_RADIO_STATUS},

    {":SYSTtem:SET:DATA:TIME",                                      SYSTEM_SET_DATA_TIME},
    {":SYSTtem:GET:DATA:TIME?",                                     SYSTEM_GET_DATA_TIME},
    {":SYSTtem:HEARt:BEAT",                                         SYSTEM_HEART_BEAT},
    {":SYSTtem:CHECk:STATe",                                        SYSTEM_CHRECK_STATE},
    {":SYSTtem:RESTart:APP",                                        SYSTEM_RESTART_APP},
    {":SYSTtem:COLLEct:RADIo:STATus",                               SYSTEM_COLLECT_RADIO_STATUS},
    {":SYSTtem:RECOrd:AUDIo",                                       SYSTEM_RECORD_AUDIO_ENABLE},
    {":SYSTtem:SET:GUID",                                           SYSTEM_SET_GUID},
    {":SYSTtem:SET:AMPLify",                                        SYSTEM_SET_AMPLIFY},
    {":SYSTtem:SET:STATe:DISTurb",                                  SYSTEM_SET_STATE_DISTURB},

};

#endif
