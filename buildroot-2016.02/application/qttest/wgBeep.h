#ifndef WGBEEP_H
#define WGBEEP_H

#include "DemoWidget.h"

QT_BEGIN_NAMESPACE
class QPushButton;
QT_END_NAMESPACE

class wgBeep : public DemoWidget
{
	Q_OBJECT

public:
    wgBeep(QWidget *parent, QString caption =  "");
    ~wgBeep();

private slots:
    void beepButtonPressed();
    void beepButtonReleased();

private:
    QPushButton * beepButton;
    void writeBeep(const char *path, int value);
};

#endif // WGBEEP_H
