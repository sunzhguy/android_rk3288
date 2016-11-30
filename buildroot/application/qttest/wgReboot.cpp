#include "wgReboot.h"
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

wgReboot::wgReboot(QWidget *parent, QString caption)
	: DemoWidget(parent, caption)
{
    QPalette palette;
    palette.setColor(this->backgroundRole(), Qt::blue);
    this->setPalette(palette);
    this->setAutoFillBackground(true);

    rebootButton = new QPushButton("Reboot ...", this);
    rebootButton->setFixedSize(QSize(200, 200));

    QHBoxLayout *layout = new QHBoxLayout;
    layout->addWidget(rebootButton);
    setLayout(layout);

    connect(rebootButton, SIGNAL(clicked()), this, SLOT(rebootButtonClick()));
}

wgReboot::~wgReboot()
{

}

void wgReboot::rebootButtonClick()
{
	system("/sbin/reboot\n");
}
