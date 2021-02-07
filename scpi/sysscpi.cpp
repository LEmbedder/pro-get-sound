/*
** scpi控制
** 2019年10月18日11:20:41
** author:Lembedder
*/
#include "sysscpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <termios.h>
#include <unistd.h>
#include <QTimer>
tSysScpi::tSysScpi()
{
    presetSystemData();
    loadSysDeviceConfig();
    guid2hex();

}
tSysScpi::~tSysScpi()
{

}

/*
 * 初始化变量
 */
void tSysScpi::presetSystemData()
{
    /* rmtp tcp*/
    sysData.gt_rmtp.HostIp= "192.168.1.4";
    sysData.gt_rmtp.HostPort = 20001;
    sysData.gt_rmtp.HostPort4Bin = 20000;
    sysData.gt_rmtp.HostPort4Sound = 20002;
    sysData.gt_rmtp.interval = 10;//10S

    /* 上报状态信息 */
    sysData.checkStatus.power   = "OK";
    sysData.checkStatus.net     = "OK";
    sysData.checkStatus.audio   = "OFF";
    sysData.checkStatus.radioConnect = "OFF";
    sysData.checkStatus.radioType = "111";
    sysData.checkStatus.collectTimeStep = 3;
    sysData.checkStatus.stausTimeStep   = 3;
    sysData.checkStatus.collectTimeState = new QTimer;
    connect(sysData.checkStatus.collectTimeState, SIGNAL(timeout()), this, SLOT(collcetTimeState()));

    //guid
    sysData.guidRoot   = "0000000-0000-0000-0000-000000";
    sysData.guidClient = "c2c118a4-5753-451d-a285-dea773c50c4e";

    /* 询问电台设备的工作状态 */
    sysData.collect_radio_data_enable = false;

    sysData.uartSelect.uartRadio = "/dev/ttymxc1";
    sysData.uartSelect.uartMic   = "/dev/ttymxc2";
    sysData.uartSelect.uartRadioBaud = 115200;
    sysData.uartSelect.uartMicBaud   = 115200;

    sysData.workMode = SELECT_RADIO_MODE;

}


/*
 * load rmtp device setting
*/
void tSysScpi::loadSysDeviceConfig()
{
    QString fileName = QCoreApplication::applicationDirPath() + "/device";
    QStringList tagList;
    if (QFile(fileName).exists())
    {
        QSettings config(fileName, QSettings::IniFormat);
        config.beginGroup("SystemConfig");
        tagList = config.childKeys();

        if(tagList.indexOf("GUID") != -1)
        {
            sysData.guidClient = config.value("GUID").toString();
        }
        else {
            saveSysDeviceConfig();
        }
        if (tagList.indexOf("stausTimeStep") != -1) {
            sysData.checkStatus.stausTimeStep = config.value("stausTimeStep").toInt();
        }
        else {
            saveSysDeviceConfig();
        }
        if (tagList.indexOf("collectTimeStep") != -1) {
            sysData.checkStatus.collectTimeStep = config.value("collectTimeStep").toInt();
        }
        else {
            saveSysDeviceConfig();
        }
        if (tagList.indexOf("uartRadioBaud") != -1) {
            sysData.uartSelect.uartRadioBaud = config.value("uartRadioBaud").toInt();
        }
        else {
            saveSysDeviceConfig();
        }
        if (tagList.indexOf("uartMicBaud") != -1) {
            sysData.uartSelect.uartMicBaud = config.value("uartMicBaud").toInt();
        }
        else {
            saveSysDeviceConfig();
        }
        if (tagList.indexOf("uartRadio") != -1) {
            sysData.uartSelect.uartRadio = config.value("uartRadio").toString();
        }
        else {
            saveSysDeviceConfig();
        }
        if (tagList.indexOf("uartMic") != -1) {
            sysData.uartSelect.uartMic = config.value("uartMic").toString();
        }
        else {
            saveSysDeviceConfig();
        }

        config.endGroup();
    }

}
/*
 * save rmtp device setting
*/
void tSysScpi::saveSysDeviceConfig(void)
{
    QString fileName = QCoreApplication::applicationDirPath() + "/device";
    QSettings setting(fileName, QSettings::IniFormat);
    //SystemConfig
    setting.beginGroup("SystemConfig");
    setting.setValue("GUID",QString(sysData.guidClient));
    setting.setValue("stausTimeStep",QString::number(sysData.checkStatus.stausTimeStep));
    setting.setValue("collectTimeStep",QString::number(sysData.checkStatus.collectTimeStep));
    setting.setValue("uartRadioBaud",QString::number(sysData.uartSelect.uartRadioBaud));
    setting.setValue("uartMicBaud",QString::number(sysData.uartSelect.uartMicBaud));
    setting.setValue("uartRadio",QString(sysData.uartSelect.uartRadio));
    setting.setValue("uartMic",QString(sysData.uartSelect.uartMic));
    setting.endGroup();
}
/* 采集板的自检/状态上报 */
void tSysScpi::collcetTimeState(void)
{
    QString directSellect = sysData.communicate_state.direct_sellect_state ? "High" : "Low";
    sysData.checkStatus.radioType = sysData.communicate_state.radio_state ? "PTT_press":"PTT_putup";
    QString data_cable = sysData.communicate_state.data_cable_state ? "Connect" : "Disconnect";

    sysData.checkStatus.allString.clear();
    sysData.checkStatus.allString.append("CheckStatus,GUID="+sysData.guidClient+",");
    sysData.checkStatus.allString.append("Power="+sysData.checkStatus.power+",");
    sysData.checkStatus.allString.append("Net="+sysData.checkStatus.net+",");
    sysData.checkStatus.allString.append("Audio="+sysData.checkStatus.audio+",");
    sysData.checkStatus.allString.append("RadioConnect="+sysData.checkStatus.radioConnect+",");
    sysData.checkStatus.allString.append("RadioType="+sysData.checkStatus.radioType+",");
    sysData.checkStatus.allString.append("DirectSellect="+directSellect+",");
//    sysData.checkStatus.allString.append("RadioType="+sysData.checkStatus.radioType+",");
    sysData.checkStatus.allString.append("DataCable="+data_cable+"\n");
    emit emitWriteData((char *)sysData.checkStatus.allString.toStdString().data(), sysData.checkStatus.allString.length());
}

/*
 * GUID TO HEX
 */
//c2 c1 18 a4-57 53-45 1d-a2 85-de a7 73 c5 0c 4e
void tSysScpi::guid2hex(void)
{
    for (int i = 0; i < 16; i++) {
        sysData.guidClienthex[i] = 0;
    }
    QStringList list = sysData.guidClient.split('-');
    QString string = list.at(0);
//    qDebug("guidtoHex:0x%x",doubleCharToHex(string.mid(6,2).toStdString().c_str()));
    sysData.guidClienthex[0] = doubleCharToHex(string.mid(6,2).toStdString().c_str());
    sysData.guidClienthex[1] = doubleCharToHex(string.mid(4,2).toStdString().c_str());
    sysData.guidClienthex[2] = doubleCharToHex(string.mid(2,2).toStdString().c_str());
    sysData.guidClienthex[3] = doubleCharToHex(string.mid(0,2).toStdString().c_str());
    string = list.at(1);
//    qDebug()<<string;
    sysData.guidClienthex[4] = doubleCharToHex(string.mid(2,2).toStdString().c_str());
    sysData.guidClienthex[5] = doubleCharToHex(string.mid(0,2).toStdString().c_str());
    string = list.at(2);
//    qDebug()<<string;
    sysData.guidClienthex[6] = doubleCharToHex(string.mid(2,2).toStdString().c_str());
    sysData.guidClienthex[7] = doubleCharToHex(string.mid(0,2).toStdString().c_str());
    string = list.at(3);
//    qDebug()<<string;
    sysData.guidClienthex[8] = doubleCharToHex(string.mid(0,2).toStdString().c_str());
    sysData.guidClienthex[9] = doubleCharToHex(string.mid(2,2).toStdString().c_str());
    string = list.at(4);
//    qDebug()<<string;
    sysData.guidClienthex[10]  = doubleCharToHex(string.mid(0,2).toStdString().c_str());
    sysData.guidClienthex[11]  = doubleCharToHex(string.mid(2,2).toStdString().c_str());
    sysData.guidClienthex[12] = doubleCharToHex(string.mid(4,2).toStdString().c_str());
    sysData.guidClienthex[13] = doubleCharToHex(string.mid(6,2).toStdString().c_str());
    sysData.guidClienthex[14] = doubleCharToHex(string.mid(8,2).toStdString().c_str());
    sysData.guidClienthex[15] = doubleCharToHex(string.mid(10,2).toStdString().c_str());
//    for (int i = 0; i < 16; i++) {
//        printf("0x%x,",sysData.guidClienthex[i]);
//    }
//    fflush(stdout);

}
unsigned char tSysScpi::charToHex(char value)
{
    unsigned char tmp = 0;
    if (value >= '0' && value <= '9')
    {
        tmp =  value - '0';
    } else if (value >= 'a' && value <= 'f')
    {
        tmp =   0x0a + (value - 'a');
    } else if (value >= 'A' && value <= 'F')
    {
        tmp =   0x0a + (value - 'A');
    }
    return tmp;
}
unsigned char tSysScpi::doubleCharToHex(const char* buff)
{
    unsigned char tmp = 0;
    unsigned char h = 0;
    unsigned char l = 0;

    h = charToHex(buff[0]);
    l = charToHex(buff[1]);
//    qDebug("h:0x%x,l:0x%x",h,l);
    tmp = (h<<4)|l;
//    qDebug("tmp:0x%x",tmp);
    return tmp;
}
