#include "wgAudio.h"
#include <QtGui>

wgAudio::wgAudio(QWidget *parent, QString caption)
	: DemoWidget(parent, caption)
{
    QPalette palette;
    palette.setColor(this->backgroundRole(), Qt::yellow);
    this->setPalette(palette);
    this->setAutoFillBackground(true);

    audioButton = new QPushButton("Play Sound ...", this);
    audioButton->setFixedSize(QSize(200, 200));

    QHBoxLayout *layout = new QHBoxLayout;
    layout->addWidget(audioButton);
    setLayout(layout);

    connect(audioButton, SIGNAL(clicked()), this, SLOT(audioButtonClick()));
}

wgAudio::~wgAudio()
{

}

void wgAudio::audioButtonClick()
{
    system("/usr/share/demo/audio.sh");
    //QSound::play("/usr/share/demo/sample.wav");
}
