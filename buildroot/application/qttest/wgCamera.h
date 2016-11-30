#ifndef WGCAMERA_H
#define WGCAMERA_H

#include <QtGui>
#include "DemoWidget.h"
#include "videodevice.h"

class wgCamera : public DemoWidget
{
	Q_OBJECT

public:
	wgCamera(QWidget *parent, QString caption =  "");
	~wgCamera();

    bool startCapture();
    bool stopCapture();

private:
    VideoDevice * vd;
    QLabel * label;
    QImage * frame;
    QTimer * timer;
    uchar * buffer;
    int width;
    int height;
    bool working;
protected:
    void paintEvent(QPaintEvent *);
    void hideEvent ( QHideEvent * event );
    void showEvent ( QShowEvent * event );
};

#endif // WGCAMERA_H
