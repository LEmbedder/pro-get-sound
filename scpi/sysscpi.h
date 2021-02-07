/*
 * sysscpi.h
 *
 *  Created on: 2013-12-2
 *      Author: Administrator
 */

#ifndef SYSSCPI_H_
#define SYSSCPI_H_

#include <QCoreApplication>
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QDateTime>
#include <QtCore/QSettings>
#include <QTimer>
#include "basescpi.h"
#include "../common/sysstruct.h"
#include "../sysvar.h"
#include "getalsadata.h"
#include <QThread>
#include "gpio_cntrl.h"

class tSysScpi : public tBaseScpi
{
    Q_OBJECT
    void presetSystemData();
    bool guidIslegal(QString string);
    unsigned char charToHex(char value);
    unsigned char doubleCharToHex(const char *buff);
public:
    void guid2hex();
    tSysScpi();
    ~tSysScpi();

    QByteArray buffer;
    GetAlsaData *getAlsaData;
    QTimer *timerCheckStatus;
    QThread   *readData;
    gpio_cntrl *gpio;

    void handleScpiCommand(QString cmdStr, int socketIndex);
    void returnScpiResult(QString value);
    void loadSysDeviceConfig();
    void saveSysDeviceConfig();
    bool guidIslegal(QByteArray array);
public slots:

signals:
    virtual void emitWriteData(char *,int);
    virtual void emitWriteDataBin(char *,int);
    virtual void emitSendData2Pc();
    virtual void emitStopSendData2Pc();
private slots:
    void collcetTimeState();
};

#endif /* SYSSCPI_H_ */
