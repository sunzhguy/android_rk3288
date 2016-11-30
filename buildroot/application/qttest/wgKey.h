#ifndef WGKEY_H
#define WGKEY_H

#include "DemoWidget.h"
#include <QLabel>

class wgKey : public DemoWidget
{
	Q_OBJECT

public:
	wgKey(QWidget *parent, QString caption =  "");
	~wgKey();

protected:
    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);

private:
    QLabel *label;
};

#endif // WGKEY_H
