#ifndef WGREBOOT_H
#define WGREBOOT_H

#include "DemoWidget.h"

QT_BEGIN_NAMESPACE
class QPushButton;
QT_END_NAMESPACE

class wgReboot : public DemoWidget
{
	Q_OBJECT

public:
	wgReboot(QWidget *parent, QString caption =  "");
	~wgReboot();

private slots:
    void rebootButtonClick();

private:
    QPushButton * rebootButton;
};

#endif // WGREBOOT_H
