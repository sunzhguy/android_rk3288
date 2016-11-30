#include <QtGui>
#include "wgLed.h"
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

wgLed::wgLed(QWidget *parent, QString caption) :
		DemoWidget(parent, caption)
{
	QPalette palette;
    palette.setColor(this->backgroundRole(), Qt::blue);
	this->setPalette(palette);
    this->setAutoFillBackground(true);

    ledButton1 = new QToolButton(this);
    if(readLed("/sys/devices/platform/leds-gpio/leds/led1/brightness") == 1)
        ledButton1->setIcon(QIcon(":/images/red.png"));
    else
        ledButton1->setIcon(QIcon(":/images/black.png"));
    ledButton1->setIconSize(QSize(96, 96));

    ledButton2 = new QToolButton(this);
    if(readLed("/sys/devices/platform/leds-gpio/leds/led2/brightness") == 1)
        ledButton2->setIcon(QIcon(":/images/red.png"));
    else
        ledButton2->setIcon(QIcon(":/images/black.png"));
    ledButton2->setIconSize(QSize(96, 96));

    ledButton3 = new QToolButton(this);
    if(readLed("/sys/devices/platform/leds-gpio/leds/led3/brightness") == 1)
        ledButton3->setIcon(QIcon(":/images/red.png"));
    else
        ledButton3->setIcon(QIcon(":/images/black.png"));
    ledButton3->setIconSize(QSize(96, 96));

    ledButton4 = new QToolButton(this);
    if(readLed("/sys/devices/platform/leds-gpio/leds/led4/brightness") == 1)
        ledButton4->setIcon(QIcon(":/images/red.png"));
    else
        ledButton4->setIcon(QIcon(":/images/black.png"));
    ledButton4->setIconSize(QSize(96, 96));

    QHBoxLayout *layout = new QHBoxLayout;
    layout->addWidget(ledButton1);
    layout->addWidget(ledButton2);
    layout->addWidget(ledButton3);
    layout->addWidget(ledButton4);
    setLayout(layout);

    connect(ledButton1, SIGNAL(clicked()), this, SLOT(ledButton1Click()));
    connect(ledButton2, SIGNAL(clicked()), this, SLOT(ledButton2Click()));
    connect(ledButton3, SIGNAL(clicked()), this, SLOT(ledButton3Click()));
    connect(ledButton4, SIGNAL(clicked()), this, SLOT(ledButton4Click()));
}

wgLed::~wgLed()
{
}

void wgLed::writeLed(const char *path, int value)
{
    int fd;

    fd = open(path, O_RDWR);
    if(fd >= 0)
    {
        char buffer[20];
        int bytes = sprintf(buffer, "%d\n", value);
        write(fd, buffer, bytes);
        ::close(fd);
    }
    else
    {
        printf("Can not write led : %s\r\n",path);
    }
}

int wgLed::readLed(const char *path)
{
    int fd;
    int v = 0;

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

void wgLed::ledButton1Click()
{
    static int flag = 0;

    flag = !flag;
    if(flag)
    {
        ledButton1->setIcon(QIcon(":/images/red.png"));
        writeLed("/sys/devices/platform/leds-gpio/leds/led1/brightness", 1);
    }
    else
    {
        ledButton1->setIcon(QIcon(":/images/black.png"));
        writeLed("/sys/devices/platform/leds-gpio/leds/led1/brightness", 0);
    }
}

void wgLed::ledButton2Click()
{
    static int flag = 0;

    flag = !flag;
    if(flag)
    {
        ledButton2->setIcon(QIcon(":/images/red.png"));
        writeLed("/sys/devices/platform/leds-gpio/leds/led2/brightness", 1);
    }
    else
    {
        ledButton2->setIcon(QIcon(":/images/black.png"));
        writeLed("/sys/devices/platform/leds-gpio/leds/led2/brightness", 0);
    }
}

void wgLed::ledButton3Click()
{
    static int flag = 0;

    flag = !flag;
    if(flag)
    {
        ledButton3->setIcon(QIcon(":/images/red.png"));
        writeLed("/sys/devices/platform/leds-gpio/leds/led3/brightness", 1);
    }
    else
    {
        ledButton3->setIcon(QIcon(":/images/black.png"));
        writeLed("/sys/devices/platform/leds-gpio/leds/led3/brightness", 0);
    }
}

void wgLed::ledButton4Click()
{
    static int flag = 0;

    flag = !flag;
    if(flag)
    {
        ledButton4->setIcon(QIcon(":/images/red.png"));
        writeLed("/sys/devices/platform/leds-gpio/leds/led4/brightness", 1);
    }
    else
    {
        ledButton4->setIcon(QIcon(":/images/black.png"));
        writeLed("/sys/devices/platform/leds-gpio/leds/led4/brightness", 0);
    }
}
