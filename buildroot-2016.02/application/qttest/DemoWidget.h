#ifndef DEMOWIDGET_H
#define DEMOWIDGET_H

#include <QWidget>
#include <QStackedWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QToolButton>
#include <QSlider>
#include <QScrollBar>
#include <QDial>
#include <QLabel>

#define EVENT_UPDATE_IMAGE 2000

class DemoWidget : public QWidget
{
	Q_OBJECT

public:
	DemoWidget(QWidget *parent = NULL , QString caption =  "", bool bShow = false);
	~DemoWidget();

	QPixmap* getImage();
	QString getCaption();

    void Init();


private:
	QPixmap * m_pImage;
	QString m_strCaption;

};

#endif // DEMOWIDGET_H
