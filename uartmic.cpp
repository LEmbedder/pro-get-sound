#include "uartmic.h"

UartMic::UartMic(QObject *parent) : QObject(parent)
{
    uart = new QSerialPort;
    initUart();
    timer = new QTimer;
    timer->setSingleShot(true);
    connect(timer, SIGNAL(timeout()), this, SLOT(changerWorkMode()));
}
void UartMic::initUart()
{
    uart->setPortName(sysData.uartSelect.uartMic);
    if (!uart->open(QIODevice::ReadWrite))
    {
        qDebug("error can not open device");
        return;
    }
    qDebug("set mic  \tdevice:%s,\tuartMicBaud:%d",
           sysData.uartSelect.uartMic.toStdString().c_str(),
           sysData.uartSelect.uartMicBaud);
    uart->setBaudRate(sysData.uartSelect.uartMicBaud, QSerialPort::AllDirections);
    uart->setDataBits(QSerialPort::Data8);
    uart->setFlowControl(QSerialPort::NoFlowControl);
    uart->setParity(QSerialPort::NoParity);
    uart->setStopBits(QSerialPort::OneStop);
    connect(uart,SIGNAL(readyRead()),this,SLOT(slots_read_uart()));

}
void UartMic::slots_write_uart(char *buff,int length)
{
    if (sysData.workMode == SELECT_MIC_MODE)
        uart->write(buff, length);
}

void UartMic::slots_read_uart()
{
    sysData.workMode = SELECT_MIC_MODE;
    qDebug()<<"SELECT_MIC_MODE";
    timer->start(3000);
    QByteArray info = uart->readAll();
    qDebug()<<info;
    emit uartMicHasdata(info.data(), info.length());
}

void UartMic::changerWorkMode(void)
{
    sysData.workMode = SELECT_RADIO_MODE;
    qDebug()<<"SELECT_RADIO_MODE";
}
