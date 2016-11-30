#include <QtGui>
#include <QLabel>
#include <QHBoxLayout>

#include "wgCamera.h"
#include "videodevice.h"
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

wgCamera::wgCamera(QWidget *parent, QString caption)
	: DemoWidget(parent, caption)
{
    QPalette palette;
    palette.setColor(this->backgroundRole(), Qt::green);
    this->setPalette(palette);
    this->setAutoFillBackground(true);

    width = 480;
    height = 360;
    working = false;
    timer = new QTimer(this);
    connect(timer,SIGNAL(timeout()),this,SLOT(update()));

    buffer = (unsigned char *) malloc(width * height * 4 * sizeof(char));
    memset(buffer, 0 ,width * height * 4 * sizeof(char));
    label = new QLabel();
    frame = new QImage(buffer, width, height, QImage::Format_RGB16);
    vd = new VideoDevice("/dev/video0", width, height);

    QHBoxLayout *hLayout = new QHBoxLayout();
    hLayout->addWidget(label);
    setLayout(hLayout);
}

bool wgCamera::startCapture()
{
    bool ret = true;

    if(vd->open_device() < 0)
    {
        vd->close_device();
        return false;
    }

    if(vd->init_device() < 0)
    {
        vd->close_device();
        return false;
    }

    if(vd->start_capturing() < 0)
    {
        vd->close_device();
        return false;
    }

    timer->start(66);
    return ret;
}

bool wgCamera::stopCapture()
{
    bool bRet = true;
    timer->stop();
    vd->stop_capturing();
    vd->uninit_device();
    vd->close_device();
    return bRet;
}

void wgCamera::hideEvent ( QHideEvent * event )
{
    if(working)
        working = !stopCapture();

    return QWidget::hideEvent(event);
}

void wgCamera::showEvent ( QShowEvent * event )
{
    working = startCapture();
    return QWidget::showEvent(event);
}

wgCamera::~wgCamera()
{
    stopCapture();

}

void wgCamera::paintEvent(QPaintEvent *)
{

    if(!working)
        return;
    uchar * p;
    size_t len;

    vd->get_frame((void **)(&p), &len);
    memcpy(buffer, p, len);

    frame->loadFromData((uchar *) buffer, width * height * 2 * sizeof(char));
    label->setUpdatesEnabled(false);
    label->setPixmap(QPixmap::fromImage(*frame, Qt::AutoColor));
    label->update();
    label->setUpdatesEnabled(true);
    vd->unget_frame();
}
