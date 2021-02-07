#ifndef GPIO_CNTRL_H
#define GPIO_CNTRL_H

#include <QObject>
#include <QtCore/QString>
#include <QtCore/QSocketNotifier>
#include <math.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/vfs.h>
#include <signal.h>
#include <termios.h>
#include "common/sysstruct.h"
#include "sysvar.h"

class gpio_cntrl : public QObject
{
    Q_OBJECT
public:
    int gpio_con;
    int imx6uirq_gpio_00;
    QSocketNotifier* imx6uirq_gpio_00_notifier;
    int imx6uirq_gpio_01;
    QSocketNotifier* imx6uirq_gpio_01_notifier;
    int imx6uirq_gpio_02;
    QSocketNotifier* imx6uirq_gpio_02_notifier;
    explicit gpio_cntrl(QObject *parent = 0);
    int gpioDownload(unsigned int cmd, unsigned long arg)
    {
        if (gpio_con > 0)
        {
            return ioctl(gpio_con, cmd, arg);
        }

        return 0;
    }
    void gpioWrite(int cmd, unsigned char *buf, int size);
    void gpioRead();
    void openSystemDevice();
    void soundChannelSelect(enum_sound_channel_select value);
    void soundAmplify(enum_sound_amplify value);
    void PTTContrl(enum_PTT_contrl value);
private:
    void test();
signals:
    virtual void emitCommunicat2PC(char *,int);
public slots:
    void readGPIO1_00();
    void readGPIO1_01();
    void readGPIO1_02();
private slots:

};

#endif // GPIO_CNTRL_H
