#include "wgBeep.h"
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

wgBeep::wgBeep(QWidget *parent, QString caption)
	: DemoWidget(parent, caption)
{
    QPalette palette;
    palette.setColor(this->backgroundRole(), Qt::yellow);
    this->setPalette(palette);
    this->setAutoFillBackground(true);

    beepButton = new QPushButton("Beep ...", this);
    beepButton->setFixedSize(QSize(200, 200));

    QHBoxLayout *layout = new QHBoxLayout;
    layout->addWidget(beepButton);
    setLayout(layout);

    connect(beepButton, SIGNAL(pressed()), this, SLOT(beepButtonPressed()));
    connect(beepButton, SIGNAL(released()), this, SLOT(beepButtonReleased()));
}

wgBeep::~wgBeep()
{

}

void wgBeep::writeBeep(const char *path, int value)
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

void wgBeep::beepButtonPressed()
{
    writeBeep("/sys/devices/platform/x4418-beep/state", 1);
}

void wgBeep::beepButtonReleased()
{
    writeBeep("/sys/devices/platform/x4418-beep/state", 0);
}
