#include "wgTouchscreen.h"
#include <QtGui>

wgTouchscreen::wgTouchscreen(QWidget *parent, QString caption)
	: DemoWidget(parent, caption)
{
    QPalette palette;
    palette.setColor (QPalette::Background, Qt::cyan);
    setPalette (palette);
    this->setAutoFillBackground(true);

    this->setMouseTracking(true);
    isPressed = false;
    m_bClicked = false;
    this->setWindowFlags(Qt::FramelessWindowHint);
}

void wgTouchscreen::onBtnCLicked()
{
    static QRect rc = this->geometry();
    if (this->width() != 1024)
    {
        this->setGeometry(0, 0, 1024, 600);
    }
    else
    {
        this->setGeometry(rc);
    }
}

wgTouchscreen::~wgTouchscreen()
{

}

void wgTouchscreen::paintEvent(QPaintEvent * p)
{
    QPainter painter(this);
    QPen pen;
    pen.setColor(Qt::red);
    pen.setWidth(2);
    painter.setPen(pen);

    for(size_t i = 0; i < lines.size(); i++)
    {
        TsLine * line = lines[i];
        painter.drawLine(line->from, line->to);
    }
    painter.end();
    return QWidget::paintEvent(p);
}

void wgTouchscreen::mousePressEvent(QMouseEvent * e)
{
    isPressed = true;
    last = e->pos();
    m_bClicked = true;
}

void wgTouchscreen::mouseMoveEvent(QMouseEvent * e)
{
    if(isPressed)
    {
        TsLine * line = new TsLine;

        line->from = last;
        line->to = e->pos();
        this->lines.push_back(line);

        last = e->pos();
        update();

        m_bClicked = false;
    }
}

void wgTouchscreen::mouseReleaseEvent(QMouseEvent * e)
{
    isPressed = false;
    last = e->pos();
    lines.clear();
    if(m_bClicked)
    {
        onBtnCLicked();
    }
    m_bClicked = false;
}

void wgTouchscreen::resizeEvent ( QResizeEvent * event )
{
    Q_UNUSED(event);
}
