#ifndef UART_H
#define UART_H

#include <QObject>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QTimer>
#include <QDebug>
#include "sysvar.h"
#include "common/sysstruct.h"

class Uart : public QObject
{
    Q_OBJECT
    void initUart();
public:
    explicit Uart(QObject *parent = 0);
    QSerialPort *uart;

signals:
    virtual void emitRadioDataToMic(char *,int);
    virtual void emitRadioDataToPC(char *,int);
public slots:
    void slots_read_uart();
    void slots_write_uart(char *buff, int length);
};

#endif // UART_H
