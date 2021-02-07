/**
 * @file gpio_cntrl.cpp
 * @author Lembedder (814699480@qq.cpm)
 * @brief 管脚控制
 * @version 0.1
 * @date 2021-01-29
 *
 * @copyright Copyright (c) 2021
 *
 */
#include "gpio_cntrl.h"

gpio_cntrl::gpio_cntrl(QObject *parent) : QObject(parent)
{
    openSystemDevice();
    soundChannelSelect(DISCONNECT);
    soundAmplify(ENBALE_AMPLIFY);
    PTTContrl(ENBALE_PTT);
//    int cmd = 102;
//    unsigned char buf[] = {1, 0};
//    gpioWrite(cmd,buf,2);
}
void gpio_cntrl::openSystemDevice(void)
{
    // 外部只采音频开关状态输入
    imx6uirq_gpio_00 = -1;
    imx6uirq_gpio_00 = open("/dev/imx6uirq_gpio_00", O_RDWR | O_NONBLOCK);
    if (imx6uirq_gpio_00 >= 0)
    {
        qDebug("load imx6uirq_gpio_00 \t\tsuccessed");
        imx6uirq_gpio_00_notifier = new QSocketNotifier(imx6uirq_gpio_00, QSocketNotifier::Read, this);
        connect(imx6uirq_gpio_00_notifier, SIGNAL(activated(int)), this, SLOT( readGPIO1_00()) );
    }
    else
    {
        qDebug("load imx6uirq_gpio_00 \t\tfailed");
    }

    // 电台PTT状态输入，即发话音状态
    imx6uirq_gpio_01 = -1;
    imx6uirq_gpio_01 = open("/dev/imx6uirq_gpio_01", O_RDWR | O_NONBLOCK);
    if (imx6uirq_gpio_01 >= 0)
    {
        qDebug("load imx6uirq_gpio_01 \t\tsuccessed");
        imx6uirq_gpio_01_notifier = new QSocketNotifier(imx6uirq_gpio_01, QSocketNotifier::Read, this);
        connect(imx6uirq_gpio_01_notifier, SIGNAL(activated(int)), this, SLOT( readGPIO1_01()) );
    }
    else
    {
        qDebug("load imx6uirq_gpio_01 \t\tfailed");
    }

    // 数据电缆连接状态输入
    imx6uirq_gpio_02 = -1;
    imx6uirq_gpio_02 = open("/dev/imx6uirq_gpio_02", O_RDWR | O_NONBLOCK);
    if (imx6uirq_gpio_02 >= 0)
    {
        qDebug("load imx6uirq_gpio_02 \t\tsuccessed");
        imx6uirq_gpio_02_notifier = new QSocketNotifier(imx6uirq_gpio_02, QSocketNotifier::Read, this);
        connect(imx6uirq_gpio_02_notifier, SIGNAL(activated(int)), this, SLOT( readGPIO1_02()) );
    }
    else
    {
        qDebug("load imx6uirq_gpio_02 \t\tfailed");
    }

    gpio_con = -1;
    gpio_con = open("/dev/gpiodtxy", O_RDWR | O_NONBLOCK);
    if (gpio_con >= 0)
    {
        qDebug("load gpiodtxy \t\tsuccessed");
    }
    else
    {
        qDebug("load gpiodtxy \t\tfailed");
    }
}


/**
 * @brief
 * 外部只采音频开关状态输入
 */
void gpio_cntrl::readGPIO1_00(void)
{
    qDebug("readGPIO1_00");
    read(imx6uirq_gpio_00, &sysData.communicate_state.direct_sellect_state, sizeof(unsigned int));
}
/**
 * @brief
 * 电台PTT状态输入，即发话音状态
 */
void gpio_cntrl::readGPIO1_01(void)
{
    qDebug("readGPIO1_01");
    read(imx6uirq_gpio_01, &sysData.communicate_state.radio_state, sizeof(unsigned int));
    if (sysData.communicate_state.radio_state == 1)
    {
        soundChannelSelect(COLLECT_SEND);
    }
    else {
        soundChannelSelect(COLLECT_RECEIVE);
    }
}
/**
 * @brief
 * 数据电缆连接状态输入
 */
void gpio_cntrl::readGPIO1_02(void)
{
    qDebug("readGPIO1_02");
    read(imx6uirq_gpio_02, &sysData.communicate_state.data_cable_state, sizeof(unsigned int));
}

/**
 * @brief 对输出管脚的控制
 *
 * @param cmd
 * @param buf
 * @param size
 */
void gpio_cntrl::gpioWrite(int cmd,unsigned char *buf,int size)
{
    gpioDownload(cmd,0);
    write(gpio_con,buf,size);
}

/**
 * @brief 控制dtxy的状态
 *	GPIO1_IO02 (X)	GPIO1_IO04 (Y)
    采收	1	        0 //COLLECT_RECEIVE
    干扰	1	        1 //DISTURB
    采发	0	        1 //COLLECT_SEND
    全断	0	        0 //DISCONNECT
 * @param value
 */
void gpio_cntrl::soundChannelSelect(enum_sound_channel_select value)
{
    unsigned char buffer[2] = {0,0};

    switch (value) {
    case COLLECT_RECEIVE:
        buffer[0] = 1;
        buffer[1] = 0;
        gpioWrite(100, buffer, sizeof(buffer));
        break;
    case DISTURB:
        buffer[0] = 1;
        buffer[1] = 1;
        gpioWrite(100, buffer, sizeof(buffer));
        break;
    case COLLECT_SEND:
        buffer[0] = 0;
        buffer[1] = 1;
        gpioWrite(100, buffer, sizeof(buffer));
        break;
    case DISCONNECT:
        buffer[0] = 0;
        buffer[1] = 0;
        gpioWrite(100, buffer, sizeof(buffer));
        break;
    default:
        break;
    }
}

/**
 * @brief
 * 音频运放选通继电器控制
    输出1时继电器动作，跳过运放
    输出0时继电器复位，选通运放
    a)上电时默认输出0，输出1时继电器动作后电台麦克风、听筒的音频信号不经过放大和增益控制，直接进入编码器；
    b)该功能使用条件需经过试验确定。
 *
 * @param value
 */
void gpio_cntrl::soundAmplify(enum_sound_amplify value)
{
    unsigned char buffer[1] = {0};

    switch (value) {
    case ENBALE_AMPLIFY:
        buffer[0] = 0;
        gpioWrite(101,buffer,sizeof(buffer));
        break;
    case DISABLE_AMPLIFY:
        buffer[0] = 1;
        gpioWrite(101,buffer,sizeof(buffer));
        break;
    default:
        break;
    }
}
/**
 * @brief
 * 电台PTT操作继电器控制
    输出1时继电器动作，控制电台PTT按下发话
    输出0时继电器复位，释放电台PTT控制
    上电时默认输出0，该功能为预留功能。
 *
 * @param value
 */
void gpio_cntrl::PTTContrl(enum_PTT_contrl value)
{
    unsigned char buffer[1] = {0};

    switch (value) {
    case ENBALE_PTT:
        buffer[0] = 0;
        gpioWrite(102,buffer,sizeof(buffer));
        break;
    case DISABLE_PTT:
        buffer[0] = 1;
        gpioWrite(102,buffer,sizeof(buffer));
        break;
    default:
        break;
    }
}
