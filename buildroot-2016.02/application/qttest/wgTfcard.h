#ifndef WGTFCARD_H
#define WGTFCARD_H

#include "DemoWidget.h"

#include <QPlainTextEdit>
#include <QProcess>

class wgTfcard : public DemoWidget
{
	Q_OBJECT

public:
    wgTfcard(QWidget *parent = NULL , QString caption =  "");
    ~wgTfcard();

private:
    QPlainTextEdit * m_pEdit;
    QProcess* cmd;

private slots:
    void onBtnTestClicked();
    void readOutput();
};

#endif // WGTFCARD_H
