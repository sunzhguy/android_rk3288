#include "DemoWidget.h"

DemoWidget::DemoWidget(QWidget *parent, QString caption, bool bShow)
	: QWidget(parent)
{
	this->setVisible(bShow);

	m_strCaption = caption;
	m_pImage = NULL;


}

void DemoWidget::Init()
{

}

DemoWidget::~DemoWidget()
{

}

QPixmap* DemoWidget::getImage()
{
	if (m_pImage)
	{
		delete m_pImage;
	}
	m_pImage = new QPixmap(this->size());
	this->render(m_pImage);
	return m_pImage;
}

QString DemoWidget::getCaption()
{
	return m_strCaption;
}

