#ifndef UARTMIC_H
#define UARTMIC_H

#include <QObject>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QTimer>
#include <QDebug>
#include "sysvar.h"
#include "common/sysstruct.h"

class UartMic : public QObject
{
    Q_OBJECT
    void initUart();
    QSerialPort *uart;
    QTimer *timer;
public:

    explicit UartMic(QObject *parent = 0);

signals:
    virtual void uartMicHasdata(char *,int);
public slots:
    void changerWorkMode();
    void slots_read_uart();
    void slots_write_uart(char *buff, int length);
};

#endif // UARTMIC_H
