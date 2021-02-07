#ifndef MAINCTRL_H
#define MAINCTRL_H

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
class MainCtrl : public QObject
{
    Q_OBJECT
public:
    int irq_dt;
    QSocketNotifier* dtNotifier;
    explicit MainCtrl(QObject *parent = 0);

signals:

public slots:
private slots:
    void readStatexy();
};

#endif // MAINCTRL_H
