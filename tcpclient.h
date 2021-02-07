#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include <QObject>
#include <QtNetwork>
#include <QTimer>
#include "sysvar.h"
#include <QCoreApplication>
#include <QSettings>
#include <QFile>
#include <QThread>
#include "scpi/sysscpi.h"

class TcpClient : public QObject
{
    Q_OBJECT
    QTcpSocket* cSocket;
    struct s_rmtp client;
    QByteArray ClientreadBuf;
    QTimer *timer;
    QTimer *timer4Bin;
    QTimer *timer4Sound;
    bool isConnect;

    void initTcpClientparams();
    void loadDeviceSetting();
    void saveDeviceSetting();

    QTcpSocket* cSocket4Bin;
    QTcpSocket* cSocket4Sound;

public:
    explicit TcpClient(QObject *parent = nullptr);

    tSysScpi* sysScpi;      //SCPI接口类
signals:
    virtual void emitWriteData2Radio(char *,int);
public slots:
    void ClientDataWrite4Sound(char *value, int len);
    void ClientDataWrite4Bin(char *value, int len);
    void ClientDataWrite(char *value, int len);
    void ClientDataWrite(QByteArray data);
protected slots:
private slots:
    void ClientDataReceived4Sound();
    void ClientDataReceived4Bin();
    void ClientDataReceived();

    bool disConnectFromServerSocket4Sound();
    bool disConnectFromServerSocket4Bin();
    bool disConnectFromServerSocket();

    bool connecToServerSocket();
    bool connecToServerSocket4Bin();
    bool connecToServerSocket4Sound();
    void hearBeat();
};

#endif // TCPCLIENT_H
