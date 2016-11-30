#ifndef QTTEST_H
#define QTTEST_H

#include <QWidget>

class qttest : public QWidget
{
	Q_OBJECT

public:
    qttest(QWidget *parent = 0);
	~qttest();

private:

};

#endif // QTTEST_H
