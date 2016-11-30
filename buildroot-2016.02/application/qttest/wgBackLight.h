#ifndef WGBACKLIGHT_H
#define WGBACKLIGHT_H

#include "DemoWidget.h"
#include "slidersgroup.h"
#include <QHBoxLayout>

#include <QCheckBox>
#include <QComboBox>
#include <QGroupBox>
#include <QLabel>
#include <QSpinBox>
#include <QStackedWidget>
#include <QVBoxLayout>

class wgBackLight : public DemoWidget
{
	Q_OBJECT

public:
	wgBackLight(QWidget *parent, QString caption =  "");
	~wgBackLight();

private:
    SlidersGroup *horizontalSliders;
    QStackedWidget *stackedWidget;
};

#endif // WGBACKLIGHT_H
