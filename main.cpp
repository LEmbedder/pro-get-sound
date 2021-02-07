#include <QCoreApplication>
#include "getalsadata.h"
#include "tcpclient.h"
#include "scpi/sysscpi.h"
#include "uart.h"
#include "mainctrl.h"
#include "gpio_cntrl.h"
#include "uartmic.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    /* 创建声音对象,获取声音需要循环所有使用新的线程 */
    QThread   *readData  = nullptr;
    readData = new QThread;
    readData->setStackSize(1024 * 1024 * 4);
    GetAlsaData *getAlsaData = new GetAlsaData;
    getAlsaData->sound_device_init();
    getAlsaData->moveToThread(readData);
    readData->start();

    /* 处理scpi命令的服务 */
    tSysScpi *sysScpi = new tSysScpi();
    sysScpi->getAlsaData = getAlsaData;
    sysScpi->readData = readData;
    /* scpi指令通知 getAlsaData 开始获取数据 SCPI -> alsa get data*/
    QObject::connect(sysScpi,SIGNAL(emitSendData2Pc()),getAlsaData,SLOT(slot_get_capture_data()));

    /* 创建网络对象,接收上位机的命令,或者发送数据给上位机*/
    TcpClient *tcpClient = nullptr;
    sysData.connect_type = TCP;
    tcpClient = new TcpClient;
    tcpClient->sysScpi = sysScpi;
    /* 返回命令端口的数据到工控机PC，是字符串形式的数据:SCPI return -> PC*/
    QObject::connect(sysScpi,SIGNAL(emitWriteData(char *,int)),tcpClient,SLOT(ClientDataWrite(char *,int)));
    /* 获取声卡数据发送给工控机PC: alse -> PC */
    QObject::connect(getAlsaData,SIGNAL(emitReadData2PC(char*,int)),tcpClient,SLOT(ClientDataWrite4Sound(char *,int)));

    /* uart和radio通信使用 */
    Uart *uart = new Uart;          /* radio_uart */
    UartMic *uartMic = new UartMic; /* Mic_uart */
    /* mic_uart 的数据到 radio_uart,只有在模式1时传输: mic_uart -> radio_uart*/
    QObject::connect(uartMic,SIGNAL(uartMicHasdata(char *,int)),uart,SLOT(slots_write_uart(char *,int)));
    /* radio_uart 的数据到 mic_uart，只有在模式1时传输: radio_uart -> mic_uart */
    QObject::connect(uart,SIGNAL(emitRadioDataToMic(char *,int)),uartMic,SLOT(slots_write_uart(char *,int)));

    /* 转发网口接收到的命令数据到radio，在工作模式0时传输: PC command -> radio command */
    QObject::connect(tcpClient,SIGNAL(emitWriteData2Radio(char*,int)),uart,SLOT(slots_write_uart(char *,int)));
    /* 转发 radio 串口的命令到工控机PC,在工作模式0时传输: radio command -> PC command */
    QObject::connect(uart,SIGNAL(emitRadioDataToPC(char *,int)),tcpClient,SLOT(ClientDataWrite4Bin(char *,int)));

    /*gpio state control*/
//    MainCtrl *mainCtrl = new MainCtrl;
    gpio_cntrl *gpioCntrl = new gpio_cntrl;
    sysScpi->gpio = gpioCntrl;
    /* 通知工控机arm工作状态 */
//    QObject::connect(gpioCntrl,SIGNAL(emitWriteData2Radio(char *,int)),tcpClient,SLOT(ClientDataWrite4Bin(char *,int)));

    return a.exec();
}
