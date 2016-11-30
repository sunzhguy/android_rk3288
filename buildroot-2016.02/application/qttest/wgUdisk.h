#ifndef WUDISK_H
#define WUDISK_H

#include "DemoWidget.h"

#include <QPlainTextEdit>
#include <QProcess>

class wgUdisk : public DemoWidget
{
	Q_OBJECT

public:
    wgUdisk(QWidget *parent = NULL , QString caption =  "");
    ~wgUdisk();

private:
    QPlainTextEdit * m_pEdit;
    QProcess* cmd;

private slots:
    void onBtnTestClicked();
    void readOutput();
};

#endif // WUDISK_H
