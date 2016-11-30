#include "qextserialport.h"
#include "qextserialenumerator.h"
#include <QTimer>
#include "wgSerial.h"
#include <QPushButton>
#include <QGridLayout>

wgSerial::wgSerial(QWidget *parent, QString caption)
	: DemoWidget(parent, caption)
{
	QPalette palette;
	palette.setColor(this->backgroundRole(), Qt::green);
	this->setPalette(palette);
    this->setAutoFillBackground(true);

    QPushButton * pButton1 = new QPushButton("Serial Test", this);
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
    InitSerialPort();
}

void wgSerial::InitSerialPort()
{
    timer = new QTimer(this);
    timer->setInterval(40);
    connect(timer, SIGNAL(timeout()), SLOT(onReadyRead()));

    PortSettings settings = {BAUD115200, DATA_8, PAR_NONE, STOP_1, FLOW_OFF, 10};
    /*
    m_pPort0 = new QextSerialPort("ttyAMA0", settings, QextSerialPort::Polling);
    m_pPort1 = new QextSerialPort("ttyAMA1", settings, QextSerialPort::Polling);
    m_pPort2 = new QextSerialPort("ttyAMA2", settings, QextSerialPort::Polling);
    m_pPort3 = new QextSerialPort("ttyAMA3", settings, QextSerialPort::Polling);
    */
    m_pPort0 = new QextSerialPort("ttyS0", settings, QextSerialPort::Polling);
    m_pPort1 = new QextSerialPort("ttyS1", settings, QextSerialPort::Polling);
    m_pPort2 = new QextSerialPort("ttyS2", settings, QextSerialPort::Polling);
    m_pPort3 = new QextSerialPort("ttyS3", settings, QextSerialPort::Polling);

}

void wgSerial::OpenSerialPOrt()
{
    if (!m_pPort0->isOpen())
    {
        m_pPort0->open(QIODevice::ReadWrite);
    }

    if (!m_pPort1->isOpen())
    {
        m_pPort1->open(QIODevice::ReadWrite);
    }

    if (!m_pPort2->isOpen())
    {
        m_pPort2->open(QIODevice::ReadWrite);
    }

    if (!m_pPort3->isOpen())
    {
        m_pPort3->open(QIODevice::ReadWrite);
    }

    timer->start();
}

void wgSerial::CloseSerialPOrt()
{
    if (m_pPort0->isOpen())
    {
        m_pPort0->close();
    }

    if (m_pPort1->isOpen())
    {
        m_pPort1->close();
    }

    if (m_pPort2->isOpen())
    {
        m_pPort2->close();
    }

    if (m_pPort3->isOpen())
    {
        m_pPort3->close();
    }

    timer->stop();
}

void wgSerial::onReadyRead()
{
    if (m_pPort0->bytesAvailable())
    {
        AddInformation(QString("[R] serial0:") + QString::fromLatin1(m_pPort0->readAll()) + QString("\n"));
    }

    if (m_pPort1->bytesAvailable())
    {
        AddInformation(QString("[R] serial1:") + QString::fromLatin1(m_pPort1->readAll()) + QString("\n"));
    }

    if (m_pPort2->bytesAvailable())
    {
        AddInformation(QString("[R] serial2:") + QString::fromLatin1(m_pPort2->readAll()) + QString("\n"));
    }

    if (m_pPort3->bytesAvailable())
    {
        AddInformation(QString("[R] serial3:") + QString::fromLatin1(m_pPort3->readAll()) + QString("\n"));
    }
}

void wgSerial::SendSerialString(QString str)
{
    if (m_pPort0->isOpen())
    {
        m_pPort0->write(str.toLatin1());
        AddInformation(QString("[S] serial0:") + str + QString("\n"));
    }

    if (m_pPort1->isOpen())
    {
        m_pPort1->write(str.toLatin1());
        AddInformation(QString("[S] serial1:") + str + QString("\n"));
    }

    if (m_pPort2->isOpen())
    {
        m_pPort2->write(str.toLatin1());
        AddInformation(QString("[S] serial2:") + str + QString("\n"));
    }

    if (m_pPort3->isOpen())
    {
        m_pPort3->write(str.toLatin1());
        AddInformation(QString("[S] serial3:") + str + QString("\n"));
    }
}

void wgSerial::AddInformation(QString  info)
{
    m_pEdit->moveCursor(QTextCursor::End);
    m_pEdit->insertPlainText(info);
}

#include <QTime>
#include <QCoreApplication>
void delay(unsigned int ms)
{
    QTime dieTime = QTime::currentTime().addMSecs(ms);
    while( QTime::currentTime() < dieTime )
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
}

void wgSerial::onBtnTestClicked()
{
    m_pEdit->clear();
    OpenSerialPOrt();
    SendSerialString("0123456789");

    delay(1000);
    CloseSerialPOrt();
}

wgSerial::~wgSerial()
{

}
