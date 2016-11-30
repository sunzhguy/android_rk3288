#ifndef WGPOWEROFF_H
#define WGPOWEROFF_H

#include "DemoWidget.h"

QT_BEGIN_NAMESPACE
class QPushButton;
QT_END_NAMESPACE

class wgPoweroff : public DemoWidget
{
	Q_OBJECT

public:
	wgPoweroff(QWidget *parent, QString caption =  "");
	~wgPoweroff();

private slots:
    void poweroffButtonClick();

private:
    QPushButton * poweroffButton;
};

#endif // WGPOWEROFF_H
