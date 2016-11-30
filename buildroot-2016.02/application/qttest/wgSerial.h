#ifndef WGSERIAL_H
#define WGSERIAL_H

#include "DemoWidget.h"
#include <QPlainTextEdit>

class QextSerialPort;
class QextSerialEnumerator;
class QTimer;

class wgSerial : public DemoWidget
{
	Q_OBJECT

public:
	wgSerial(QWidget *parent = NULL, QString caption =  "");
	~wgSerial();

private:
    QPlainTextEdit * m_pEdit;

    QextSerialPort * m_pPort0;
    QextSerialPort * m_pPort1;
    QextSerialPort * m_pPort2;
    QextSerialPort * m_pPort3;

    QTimer *timer;

    void InitSerialPort();
    void OpenSerialPOrt();
    void CloseSerialPOrt();
    void SendSerialString(QString str);

    void AddInformation(QString  info);
private slots:
    void onBtnTestClicked();
    void onReadyRead();
};

#endif // WGSERIAL_H
