#include "tcpclient.h"
//自己调试使用
#define TCP_DEBUG 1

TcpClient::TcpClient(QObject *parent) : QObject(parent)
{
    cSocket = new QTcpSocket(this);

    isConnect = false;
    loadDeviceSetting();
    connecToServerSocket();
    connect(cSocket,SIGNAL(readyRead()),this,SLOT(ClientDataReceived()));
    connect(cSocket,SIGNAL(disconnected()),this,SLOT(disConnectFromServerSocket()));
    connect(cSocket,SIGNAL(error(QAbstractSocket::SocketError)),this,SLOT(disConnectFromServerSocket()));

    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(connecToServerSocket()));
#if (TCP_DEBUG == 1)
    timer->start(1000*10);
#endif
    /* 定时给工控机发送消息 */
    timer4Bin = new QTimer(this);
    connect(timer4Bin, SIGNAL(timeout()), this, SLOT(hearBeat()));
    timer4Bin->start(1000*30);

    cSocket4Bin = new QTcpSocket(this);
    connecToServerSocket4Bin();
    connect(cSocket4Bin,SIGNAL(readyRead()),this,SLOT(ClientDataReceived4Bin()));
    connect(cSocket4Bin,SIGNAL(disconnected()),this,SLOT(disConnectFromServerSocket4Bin()));
    connect(cSocket4Bin,SIGNAL(error(QAbstractSocket::SocketError)),this,SLOT(disConnectFromServerSocket4Bin()));

    /* 发送sound数据的端口 */
    cSocket4Sound = new QTcpSocket(this);
    connecToServerSocket4Sound();
    connect(cSocket4Sound,SIGNAL(readyRead()),this,SLOT(ClientDataReceived4Sound()));
    connect(cSocket4Sound,SIGNAL(disconnected()),this,SLOT(disConnectFromServerSocket4Sound()));
    connect(cSocket4Sound,SIGNAL(error(QAbstractSocket::SocketError)),this,SLOT(disConnectFromServerSocket4Sound()));
    /* 定时连接上位机的音频端口 */
    timer4Sound = new QTimer(this);
//     connect(timer4Sound, SIGNAL(timeout()), this, SLOT(hearBeat()));
//    timer4Sound->start(1000*30);
}
/**
 * @brief
 *
 * @return true
 * @return false
 */
bool TcpClient::connecToServerSocket(void)
{
    disConnectFromServerSocket();
    qDebug()<<"connect HostIp:"<<client.HostIp;
    cSocket->connectToHost(client.HostIp, client.HostPort, QTcpSocket::ReadWrite);
    if (cSocket->waitForConnected(1000))
    {
        isConnect = true;
        qDebug("connect HostPort:%d \t\tok",client.HostPort);
        return true;
    }
    else {
        qDebug("connect HostPort \tfailed");
        return false;
    }
}
/* connect for bin data */
bool TcpClient::connecToServerSocket4Bin(void)
{
    disConnectFromServerSocket4Bin();
    cSocket4Bin->connectToHost(client.HostIp, client.HostPort4Bin, QTcpSocket::ReadWrite);
    if (cSocket4Bin->waitForConnected(1000))
    {
        qDebug("connect cSocket4Bin:%d \tok",client.HostPort4Bin);
        return true;
    }
    else {
        qDebug("connect cSocket4Bin \tfailed");
        return false;
    }
}
bool TcpClient::connecToServerSocket4Sound(void)
{
    disConnectFromServerSocket4Sound();
    cSocket4Sound->connectToHost(client.HostIp, client.HostPort4Sound, QTcpSocket::ReadWrite);
    if (cSocket4Sound->waitForConnected(100))
    {
        qDebug("connect cSocket4Sound:%d \tok",client.HostPort4Sound);
        return true;
    }
    else {
        qDebug("connect cSocket4Sound \tfailed");
        return false;
    }
}
/*
 * 接收网卡的二进制数据
*/
void TcpClient::ClientDataReceived4Bin()
{
    QByteArray datagram;
    while (cSocket4Bin->bytesAvailable() > 0)
    {
         datagram = cSocket4Bin->readAll();
    }
    if (sysData.workMode == SELECT_MIC_MODE) return;//work_mode == 1
//        for (int i = 0;i < datagram.length(); i++) {
//            printf("0x%02x ",datagram.at(i));
//        }
//    fflush(stdout);
    uint8_t head = datagram.mid(0,1).toInt()&0xff;
    datagram.remove(0,1);
    uint16_t length = datagram.at(0) | datagram.at(1) << 8;
    datagram.remove(0,2);
    QByteArray guid = datagram.mid(0,16);
    datagram.remove(0,16);
    uint8_t dataType = datagram.at(0)&0xff;
    if(dataType == HEAR_BEAT) {
        /* 接收到了心跳包，这里什么也不做 */
        return;
    }
    datagram.remove(0,1);
    QByteArray data = datagram.mid(0,datagram.length()-1);
    datagram.remove(0,datagram.length()-1);
    uint8_t end = datagram.at(0)&0xff;
    qDebug()<<QString(data);
    for (int i = 0;i < data.length(); i++) {
        printf("0x%02x ",data.at(i));
    }
    printf("\r\n");
    fflush(stdout);
    qDebug("length:%d,dataType:0x%x,end:0x%x",length,dataType,end);
    if (!sysScpi->guidIslegal(guid))
        return;
    if (dataType == REMOTE_RADIO_COMMANDTO_PC_TO_DEVICE__TYPE) {
        emit emitWriteData2Radio(data.data(), data.length());
    }
}
/**
 * @brief 接受音频数据的端口
 *
 */
void TcpClient::ClientDataReceived4Sound()
{
    while (cSocket4Sound->bytesAvailable() > 0)
    {
        QByteArray datagram = cSocket4Sound->readAll();
    }
}
/**
 * @brief sound端口的写出数据 for sound
 *
 * @param value
 * @param len
 */
void TcpClient::ClientDataWrite4Sound(char *value,int len)
{
    if (value != NULL && len > 0 && cSocket4Sound != NULL &&
                    sysData.workMode == SELECT_RADIO_MODE) {
        cSocket4Sound->write(value, len);
        if (!cSocket4Sound->waitForBytesWritten(300)) {
            connecToServerSocket4Sound();
        }
    }
}
/**
 * 写出数据 for bin data
*/
void TcpClient::ClientDataWrite4Bin(char *value,int len)
{
    if (value != NULL && len > 0 && cSocket4Bin != NULL &&
            sysData.workMode == SELECT_RADIO_MODE)
    {
        cSocket4Bin->write(value, len);
        if (!cSocket4Bin->waitForBytesWritten(300))
        {
            connecToServerSocket4Bin();
        }
    }
}
/**
 * 断开与服务器的连接
*/
bool TcpClient::disConnectFromServerSocket4Bin(void)
{
    cSocket4Bin->disconnectFromHost();
    return true;
}
/**
 * @brief 断开与发送音频数据端口的服务器连接
 *
 * @return true
 * @return false
 */
bool TcpClient::disConnectFromServerSocket4Sound(void)
{
    cSocket4Sound->disconnectFromHost();
    return true;
}
/**
 * 给工控机发送心跳包
*/
void TcpClient::hearBeat(void)
{
    char data[21] = {0xff,0x13,0x00,0};
    memcpy(&data[3],sysData.guidClienthex,16);
    data[19] = 0xef;
    data[20] = 0xff;
    ClientDataWrite4Bin(data,sizeof(data));
}
//断开与服务器的连接
bool TcpClient::disConnectFromServerSocket(void)
{
    cSocket->disconnectFromHost();
    isConnect = false;
    return true;
}

/*Tcp Client 读取数据*/
void TcpClient::ClientDataReceived(void)
{
#if (TCP_DEBUG == 1)
    timer->start(1000*client.interval);
#endif
    while (cSocket->bytesAvailable() > 0)
    {
        ClientreadBuf = cSocket->readAll();
//        QByteArray datagram;
//        datagram.resize(cSocket->bytesAvailable());
//        cSocket->read(datagram.data(),datagram.size());
//        QString msg = datagram.data();
//        qDebug()<<msg;
//        ClientDataWrite(ClientreadBuf.data(),ClientreadBuf.length());
    }
    /* command process */
    sysScpi->handleScpiCommand((QString)ClientreadBuf, 0);
}
void TcpClient::ClientDataWrite(QByteArray data)
{
    ClientDataWrite(data.data(), data.length());
}
/* 写出数据 */
void TcpClient::ClientDataWrite(char *value,int len)
{
    if (value != NULL && len > 0 && cSocket != NULL && (isConnect == true) &&
            sysData.workMode == SELECT_RADIO_MODE)
    {
        cSocket->write(value, len);
        if (!cSocket->waitForBytesWritten(30))
        {
            //connecToServerSocket();
        }
        return;
    }
}
/*
 * load rmtp device setting
*/
void TcpClient::loadDeviceSetting()
{
    QString fileName = QCoreApplication::applicationDirPath() + "/device";
    QStringList tagList;
    if (QFile(fileName).exists())
    {
        QSettings setting(fileName, QSettings::IniFormat);
        setting.beginGroup("TcpClient");
        tagList = setting.childKeys();

        if(tagList.indexOf("HostIP") != -1)
        {
            client.HostIp = setting.value("HostIP").toString();
        }
        if(tagList.indexOf("HostPort") != -1)
        {
            client.HostPort  = setting.value("HostPort").toInt();
        }
        if(tagList.indexOf("HostPort4Bin") != -1)
        {
            client.HostPort4Bin  = setting.value("HostPort4Bin").toInt();
        }else
            saveDeviceSetting();
        if(tagList.indexOf("HostPort4Sound") != -1)
        {
            client.HostPort4Sound  = setting.value("HostPort4Sound").toInt();
        }else
            saveDeviceSetting();
        if(tagList.indexOf("Interval") != -1)
        {
            client.interval = setting.value("Interval").toInt();
            if(client.interval < 10)
            {
                client.interval = 10;
            }
        }
        if(tagList.indexOf("enable") != -1)
        {
            client.enable  = setting.value("enable").toBool();
        }
        setting.endGroup();

    } else {
        saveDeviceSetting();
    }
}
/*
 * save rmtp device setting
*/
void TcpClient::saveDeviceSetting(void)
{
    QString fileName = QCoreApplication::applicationDirPath() + "/device";
    QSettings setting(fileName, QSettings::IniFormat);
    //TcpClient
    setting.beginGroup("TcpClient");
    setting.setValue("HostIP",QString(sysData.gt_rmtp.HostIp));
    setting.setValue("HostPort",sysData.gt_rmtp.HostPort);
    setting.setValue("HostPort4Bin",sysData.gt_rmtp.HostPort4Bin);
    setting.setValue("HostPort4Sound",sysData.gt_rmtp.HostPort4Sound);
    setting.setValue("Interval",sysData.gt_rmtp.interval);
    setting.setValue("enable",QString(sysData.gt_rmtp.enable));
    setting.endGroup();
}
