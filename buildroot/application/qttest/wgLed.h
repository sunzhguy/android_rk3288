#ifndef WGLED_H
#define WGLED_H

#include "DemoWidget.h"

QT_BEGIN_NAMESPACE
class QToolButton;
QT_END_NAMESPACE

class wgLed : public DemoWidget
{
	Q_OBJECT

public:
	wgLed(QWidget *parent = NULL , QString caption =  "");
	~wgLed();

private slots:
    void ledButton1Click();
    void ledButton2Click();
    void ledButton3Click();
    void ledButton4Click();

private:
    QToolButton * ledButton1;
    QToolButton * ledButton2;
    QToolButton * ledButton3;
    QToolButton * ledButton4;
    void writeLed(const char *path, int value);
    int readLed(const char *path);
};

#endif // WGLED_H
