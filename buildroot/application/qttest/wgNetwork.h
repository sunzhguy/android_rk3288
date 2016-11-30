#ifndef WGNETWORK_H
#define WGNETWORK_H

#include "DemoWidget.h"

#include <QPlainTextEdit>
#include <QProcess>

class wgNetwork : public DemoWidget
{
	Q_OBJECT

public:
    wgNetwork(QWidget *parent = NULL , QString caption =  "");
    ~wgNetwork();

private:
    QPlainTextEdit * m_pEdit;
    QProcess* cmd;

private slots:
    void onBtnTestClicked();
    void readOutput();
};

#endif // WGNETWORK_H
