#ifndef WGDEEPSLEEP_H
#define WGDEEPSLEEP_H

#include "DemoWidget.h"
#include <QPushButton>

class wgDeepSleep : public DemoWidget
{
	Q_OBJECT

public:
	wgDeepSleep(QWidget *parent, QString caption =  "");
	~wgDeepSleep();

private slots:
    void sleepButtonClick();

private:
    QPushButton * sleepButton;
    void delay(unsigned int ms);
    void goToDeepSleep();
};

#endif // WGDEEPSLEEP_H
