/**
 * @file mainctrl.cpp
 * @author Lembedder (814699480@qq.cpm)
 * @brief 输入管脚的控制
 * @version 0.1
 * @date 2021-01-29
 *
 * @copyright Copyright (c) 2021
 *
 */
#include "mainctrl.h"

MainCtrl::MainCtrl(QObject *parent) : QObject(parent)
{
    /* 中频驱动 */
    irq_dt = -1;
    irq_dt = open("/dev/imx6uirq_dt", O_RDWR | O_NONBLOCK);
    if (irq_dt >= 0) {
        qDebug("load imx6uirq_dt \tsuccessed");
        dtNotifier = new QSocketNotifier(irq_dt, QSocketNotifier::Read, this);
        connect(dtNotifier, SIGNAL(activated(int)), this, SLOT( readStatexy()) );
    } else {
        qDebug("load imx6uirq_dt \tfailed");
    }
}

void MainCtrl::readStatexy(void)
{
    /*  */
    qDebug("readStatexy");

}
