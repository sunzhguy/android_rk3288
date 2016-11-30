#include "wgPoweroff.h"
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

wgPoweroff::wgPoweroff(QWidget *parent, QString caption)
	: DemoWidget(parent, caption)
{
    QPalette palette;
    palette.setColor(this->backgroundRole(), Qt::green);
    this->setPalette(palette);
    this->setAutoFillBackground(true);

    poweroffButton = new QPushButton("Poweroff ...", this);
    poweroffButton->setFixedSize(QSize(200, 200));

    QHBoxLayout *layout = new QHBoxLayout;
    layout->addWidget(poweroffButton);
    setLayout(layout);

    connect(poweroffButton, SIGNAL(clicked()), this, SLOT(poweroffButtonClick()));
}

wgPoweroff::~wgPoweroff()
{

}

void wgPoweroff::poweroffButtonClick()
{
	system("/sbin/poweroff\n");
}
