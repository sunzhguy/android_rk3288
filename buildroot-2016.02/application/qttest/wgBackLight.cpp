#include <QtGui>
#include "wgBackLight.h"
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

wgBackLight::wgBackLight(QWidget *parent, QString caption) :
		DemoWidget(parent, caption)
{
	QPalette palette;
    palette.setColor(this->backgroundRole(), Qt::magenta);
	this->setPalette(palette);
    this->setAutoFillBackground(true);

    horizontalSliders = new SlidersGroup(Qt::Horizontal, tr("brightness: 255"));
    horizontalSliders->setMinimum(0);
    horizontalSliders->setMaximum(255);
    horizontalSliders->setValue(255);

    stackedWidget = new QStackedWidget;
    stackedWidget->addWidget(horizontalSliders);

    QHBoxLayout *layout = new QHBoxLayout;
    layout->addWidget(stackedWidget);
    setLayout(layout);
}

wgBackLight::~wgBackLight()
{

}
