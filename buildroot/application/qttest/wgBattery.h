#ifndef WGBATTERY_H
#define WGBATTERY_H

#include "DemoWidget.h"
#include <QLabel>
#include <QTimer>

class wgBattery : public DemoWidget
{
	Q_OBJECT

public:
	wgBattery(QWidget *parent, QString caption =  "");
	~wgBattery();

private slots:
    void timerUpdate();

private:
    QLabel *ac_online;
    QLabel *battery_vol;
    QLabel *battery_capacity;

    QTimer *timer;
    int readBattery(const char *path);
    char * readBatteryString(const char *path, char *result);
};

#endif // WGBATTERY_H
