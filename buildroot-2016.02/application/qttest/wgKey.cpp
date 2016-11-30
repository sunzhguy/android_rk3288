#include "wgKey.h"
#include <QtGui>
#include <QKeyEvent>
extern "C"
{
#include <stdio.h>
}

wgKey::wgKey(QWidget *parent, QString caption)
	: DemoWidget(parent, caption)
{
    QPalette palette;
    palette.setColor(this->backgroundRole(), Qt::cyan);
    this->setPalette(palette);
    this->setAutoFillBackground(true);

    label = new QLabel("Press any key!");
    QHBoxLayout *layout = new QHBoxLayout;
    layout->addWidget(label);
    setLayout(layout);
}

wgKey::~wgKey()
{

}

void wgKey::keyPressEvent(QKeyEvent *event)
{
    label->setText("[keydown] : 0x" + QString::number(event->key(), 16));
}

void wgKey::keyReleaseEvent(QKeyEvent *event)
{
    label->setText("[keyup] : 0x" + QString::number(event->key(),16));
}
