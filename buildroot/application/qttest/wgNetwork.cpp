#include <QtGui>
#include "wgNetwork.h"
extern "C"
{
#include <fcntl.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <unistd.h>
}

wgNetwork::wgNetwork(QWidget *parent, QString caption) :
		DemoWidget(parent, caption)
{
	QPalette palette;
    palette.setColor(this->backgroundRole(), Qt::blue);
	this->setPalette(palette);
    this->setAutoFillBackground(true);

    QPushButton * pButton1 = new QPushButton("Network Test", this);
    QObject::connect(pButton1, SIGNAL(clicked()), this, SLOT(onBtnTestClicked()));
    QSizePolicy sizePolicy1(QSizePolicy::Minimum, QSizePolicy::Expanding);
    sizePolicy1.setHorizontalStretch(0);
    sizePolicy1.setVerticalStretch(0);
    sizePolicy1.setHeightForWidth(pButton1->sizePolicy().hasHeightForWidth());
    pButton1->setSizePolicy(sizePolicy1);

    m_pEdit = new QPlainTextEdit(this);
    m_pEdit->setMaximumBlockCount(800);

    QVBoxLayout *verticalLayout = new QVBoxLayout();
    verticalLayout->setSpacing(6);
    verticalLayout->addWidget(m_pEdit);
    verticalLayout->addWidget(pButton1);

    verticalLayout->setStretch(0, 4);
    verticalLayout->setStretch(1, 2);

    setLayout(verticalLayout);

    cmd = new QProcess;
    connect(cmd, SIGNAL(readyRead()), this, SLOT(readOutput()));
}

wgNetwork::~wgNetwork()
{
}

void wgNetwork::onBtnTestClicked()
{
    m_pEdit->clear();
    cmd->start("/usr/share/demo/network.sh");
}

void wgNetwork::readOutput()
{
    QString output = cmd->readAll();
    m_pEdit->moveCursor(QTextCursor::End);
    m_pEdit->insertPlainText(output);
}
