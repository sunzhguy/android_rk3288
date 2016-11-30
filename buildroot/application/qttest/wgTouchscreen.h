#ifndef WGTOUCHSCREEN_H
#define WGTOUCHSCREEN_H

using namespace std;

#include "DemoWidget.h"
#include <QPainter>
#include <QPoint>
#include <QMouseEvent>
#include <vector>
#include <QLabel>

typedef struct TsLine{
    QPoint from;
    QPoint to;
}TsLine;

class wgTouchscreen : public DemoWidget
{
	Q_OBJECT

public:
	wgTouchscreen(QWidget *parent, QString caption =  "");
	~wgTouchscreen();

protected:
    vector<TsLine*> lines;
    bool isPressed;
    QPoint last;
    bool m_bClicked;

    void paintEvent(QPaintEvent * p);
    void mousePressEvent(QMouseEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);
    void resizeEvent ( QResizeEvent * event );

private slots:
    void onBtnCLicked();
};

#endif // WGTOUCHSCREEN_H
