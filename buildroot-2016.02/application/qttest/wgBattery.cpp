#include <QtGui>
#include "wgBattery.h"
#include <QVBoxLayout>

extern "C"
{
#include <fcntl.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <unistd.h>
}

wgBattery::wgBattery(QWidget *parent, QString caption)
	: DemoWidget(parent, caption)
{
    QPalette palette;
    palette.setColor(this->backgroundRole(), Qt::yellow);
    this->setPalette(palette);
    this->setAutoFillBackground(true);

    ac_online = new QLabel("AC Online: ");
    battery_vol = new QLabel("Battery Vol: ");
    battery_capacity = new QLabel("Battery Capacity: ");

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(ac_online);
    layout->addWidget(battery_vol);
    layout->addWidget(battery_capacity);
    setLayout(layout);

    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(timerUpdate()));
    timer->start(1000);
}

wgBattery::~wgBattery()
{

}

int wgBattery::readBattery(const char *path)
{
    int fd;
    int v = -1;

    fd = open(path, O_RDONLY);
    if(fd >= 0)
    {
        char buffer[20];
        if(read(fd, buffer, sizeof(buffer)) > 0)
        {
            sscanf(buffer, "%d\n", &v);
        }
        ::close(fd);
    }

    return v;
}

char * wgBattery::readBatteryString(const char *path, char *result)
{
    int fd;

    fd = open(path, O_RDONLY);
    if(fd >= 0)
    {
        char buffer[20];
        if(read(fd, buffer, sizeof(buffer)) > 0)
        {
            sscanf(buffer, "%s\n", result);
        }
        ::close(fd);
    }

    return result;
}

void wgBattery::timerUpdate()
{
    char buffer[128];
    int v;

    ac_online->setText("AC Online: " + QString::fromUtf8(readBatteryString("//sys/devices/platform/i2c-gpio.3/i2c-3/3-0034/axp22-supplyer.19/power_supply/battery/present", buffer)));

    v = readBattery("/sys/devices/platform/i2c-gpio.3/i2c-3/3-0034/axp22-supplyer.19/power_supply/battery/voltage_now");
    battery_vol->setText("Battery Vol: " + QString::number(v));

    v = readBattery("/sys/devices/platform/i2c-gpio.3/i2c-3/3-0034/axp22-supplyer.19/power_supply/battery/capacity");
    battery_capacity->setText("Battery Capacity: " + QString::number(v) + "%");
}
