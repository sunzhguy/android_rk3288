#include "wgDeepSleep.h"
#include <QtGui>
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

wgDeepSleep::wgDeepSleep(QWidget *parent, QString caption)
	: DemoWidget(parent, caption)
{
    QPalette palette;
    palette.setColor(this->backgroundRole(), Qt::red);
    this->setPalette(palette);
    this->setAutoFillBackground(true);

    sleepButton = new QPushButton("Deep Sleep ...", this);
    sleepButton->setFixedSize(QSize(200, 200));

    QHBoxLayout *layout = new QHBoxLayout;
    layout->addWidget(sleepButton);
    setLayout(layout);

    connect(sleepButton, SIGNAL(clicked()), this, SLOT(sleepButtonClick()));
}

wgDeepSleep::~wgDeepSleep()
{

}

void wgDeepSleep::delay(unsigned int ms)
{
    QTime dieTime = QTime::currentTime().addMSecs(ms);
    while( QTime::currentTime() < dieTime )
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
}

void wgDeepSleep::goToDeepSleep()
{
    char buffer[20];
    int bytes;
    int fd;

    fd = open("/sys/power/state", O_RDWR);
    if(fd >= 0)
    {

        bytes = sprintf(buffer, "%s\n", "on");
        write(fd, buffer, bytes);


        bytes = sprintf(buffer, "%s\n", "mem");
        write(fd, buffer, bytes);


        delay(3000);


        bytes = sprintf(buffer, "%s\n", "on");
        write(fd, buffer, bytes);

        ::close(fd);
    }
    else
    {
        printf("Can not deep sleep\n");
    }
}

void wgDeepSleep::sleepButtonClick()
{
    printf("[DEEP SLEEP]go to deep sleep ...\n");
    system("/sbin/ifconfig eth0 down\n");
    goToDeepSleep();
    system("/sbin/ifconfig eth0 up\n");
    printf("[DEEP SLEEP]system wakeup ...\n");
}
