#ifndef WGAUDIO_H
#define WGAUDIO_H

#include "DemoWidget.h"
#include <QProcess>

QT_BEGIN_NAMESPACE
class QPushButton;
QT_END_NAMESPACE

class wgAudio : public DemoWidget
{
	Q_OBJECT

public:
	wgAudio(QWidget *parent, QString caption =  "");
	~wgAudio();

private slots:
	void audioButtonClick();

private:
	QPushButton * audioButton;
};

#endif // WGAUDIO_H
