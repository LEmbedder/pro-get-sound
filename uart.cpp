/***/
#include "uart.h"

Uart::Uart(QObject *parent) : QObject(parent)
{
    uart = new QSerialPort;
    initUart();

}

void Uart::initUart()
{
    uart->setPortName(sysData.uartSelect.uartRadio.toStdString().c_str());
    if (!uart->open(QIODevice::ReadWrite))
    {
        qDebug("error can not open device");
        return;
    }
    qDebug("set radio\tdevice:%s,\tBaud:%d",
           sysData.uartSelect.uartRadio.toStdString().c_str(),
           sysData.uartSelect.uartRadioBaud);
    uart->setBaudRate(sysData.uartSelect.uartRadioBaud, QSerialPort::AllDirections);
    uart->setDataBits(QSerialPort::Data8);
    uart->setFlowControl(QSerialPort::NoFlowControl);
    uart->setParity(QSerialPort::NoParity);
    uart->setStopBits(QSerialPort::OneStop);
    connect(uart,SIGNAL(readyRead()),this,SLOT(slots_read_uart()));
    /////////////test//////////////
//    QByteArray data;
//    QByteArray info("test");
//    data.append(0xFF).append(info.length()+18).append(QByteArray(sysData.guidClienthex))
//            .append(REMOTE_RADIO_COMMANDTO_DEVICE_TO_PC__TYPE).append(info)
//            .append(0xFF);
//    qDebug()<<data;
}

void Uart::slots_write_uart(char *buff,int length)
{
    qDebug("uart_radio write data\r\n");
//    for (int i = 0;i < length; i++) {
//        printf("0x%02x ",buff[i]);
//    }
//    printf("\r\n");
//    fflush(stdout);
    uart->write(buff, length);
}

void Uart::slots_read_uart()
{
    QByteArray info = uart->readAll();
    if (sysData.workMode == SELECT_MIC_MODE)
        emit emitRadioDataToMic(info.data(), info.length());
    else
    {
        QByteArray data;
        QByteArray guid;
        data.clear();
        guid.clear();
        guid.insert(0,sysData.guidClienthex,16);

        unsigned short length = info.length()+16+2+1;
        data.append(0xFF).append(length&0XFF).append((length >>8) &0Xff)
                .append(guid)
                .append(REMOTE_RADIO_COMMANDTO_DEVICE_TO_PC__TYPE).append(info)
                .append(0xFF);
//        qDebug()<<info.toHex();
//        qDebug()<<guid.toHex();
        emit emitRadioDataToPC(data.data(), data.length());
    }

}
