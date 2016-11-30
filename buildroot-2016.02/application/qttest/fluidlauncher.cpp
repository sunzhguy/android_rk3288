/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the demonstration applications of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QXmlStreamReader>

#include "fluidlauncher.h"
#include "DemoWidget.h"
#include "wgLed.h"
#include "wgSerial.h"
#include "wgBackLight.h"
#include "wgBattery.h"
#include "wgCamera.h"
#include "wgKey.h"
#include "wgBeep.h"
#include "wgAudio.h"
#include "wgTouchscreen.h"
#include "wgNetwork.h"
#include "wgTfcard.h"
#include "wgUdisk.h"
#include "wgDeepSleep.h"
#include "wgReboot.h"
#include "wgPoweroff.h"

#define DEFAULT_INPUT_TIMEOUT 60000
#define SIZING_FACTOR_HEIGHT 6/10
#define SIZING_FACTOR_WIDTH 6/10

//QPixmap * pImage = NULL;
FluidLauncher::FluidLauncher(QStringList* args)
{
    Q_UNUSED(args);
    pictureFlowWidget = new PictureFlow();
    slideShowWidget = new SlideShow();
    inputTimer = new QTimer();

    addWidget(pictureFlowWidget);
    addWidget(slideShowWidget);
    setCurrentWidget(pictureFlowWidget);
    pictureFlowWidget->setFocus();

    QRect screen_size = QApplication::desktop()->screenGeometry();

    //QObject::connect(pictureFlowWidget, SIGNAL(itemActivated(int)), this, SLOT(launchApplication(int)));
	QObject::connect(pictureFlowWidget, SIGNAL(sigAnimateDone(int, bool)), this, SLOT(onAnimateDone(int, bool)));
    QObject::connect(pictureFlowWidget, SIGNAL(inputReceived()),    this, SLOT(resetInputTimeout()));
    QObject::connect(slideShowWidget,   SIGNAL(inputReceived()),    this, SLOT(switchToLauncher()));
    QObject::connect(inputTimer,        SIGNAL(timeout()),          this, SLOT(inputTimedout()));

    inputTimer->setSingleShot(true);
    inputTimer->setInterval(DEFAULT_INPUT_TIMEOUT);

    const int h = screen_size.height() * SIZING_FACTOR_HEIGHT;
    const int w = screen_size.width() * SIZING_FACTOR_WIDTH;
    const int hh = qMin(h, w);
    const int ww = hh / 3 * 3.5;
    pictureFlowWidget->setSlideSize(QSize(ww, hh));

	showFullScreen();
}

void FluidLauncher::onAnimateDone(int index, bool bStart)
{
	if (bStart)
	{
		demoList[index]->hide();
	}
	else
	{
		demoList[index]->show();
		demoList[index]->raise();
		demoList[index]->activateWindow();
        demoList[index]->setFocus();
	}
}

void FluidLauncher::switchToPictureFlow()
{
	parseSlideshow();
	loadConfig("");
	populatePictureFlow();


	inputTimer->start();
}

FluidLauncher::~FluidLauncher()
{
    delete pictureFlowWidget;
    delete slideShowWidget;
}

bool FluidLauncher::loadConfig(QString configPath)
{
    Q_UNUSED(configPath);
    QRect screenRect = QApplication::desktop()->screenGeometry();
    int x = (screenRect.width() - pictureFlowWidget->slideSize().width()) / 2 - 3;
    int y = (screenRect.height() - pictureFlowWidget->slideSize().height()) / 4 - 2;
    //QRect demoRect(229, 46, pictureFlowWidget->slideSize().width(), pictureFlowWidget->slideSize().height());
    QRect demoRect(x, y, pictureFlowWidget->slideSize().width(), pictureFlowWidget->slideSize().height());
    DemoWidget * pWidget = new wgLed(pictureFlowWidget, "LED Test");
    pWidget->installEventFilter(this);
	pWidget->setGeometry(demoRect);
    demoList.append(pWidget);

    pWidget = new wgBeep(pictureFlowWidget, "Beep Test");
    pWidget->installEventFilter(this);
    pWidget->setGeometry(demoRect);
    demoList.append(pWidget);

    pWidget = new wgBackLight(pictureFlowWidget, "Backlight Test");
    pWidget->installEventFilter(this);
    pWidget->setGeometry(demoRect);
    demoList.append(pWidget);

    pWidget = new wgKey(pictureFlowWidget, "KeyBoard Test");
    pWidget->installEventFilter(this);
    pWidget->setGeometry(demoRect);
    demoList.append(pWidget);

    pWidget = new wgBattery(pictureFlowWidget, "Battery Test");
    pWidget->installEventFilter(this);
    pWidget->setGeometry(demoRect);
    demoList.append(pWidget);

//    pWidget = new wgCamera(pictureFlowWidget, "Camera Test");
//    pWidget->installEventFilter(this);
//    pWidget->setGeometry(demoRect);
//    demoList.append(pWidget);

    pWidget = new wgAudio(pictureFlowWidget, "Audio Test");
    pWidget->installEventFilter(this);
    pWidget->setGeometry(demoRect);
    demoList.append(pWidget);

    pWidget = new wgTouchscreen(pictureFlowWidget, "TouchScreen Test");
    pWidget->installEventFilter(this);
    pWidget->setGeometry(demoRect);
    demoList.append(pWidget);

    pWidget = new wgSerial(pictureFlowWidget, "Serial Test");
    pWidget->installEventFilter(this);
	pWidget->setGeometry(demoRect);
	demoList.append(pWidget);

    pWidget = new wgNetwork(pictureFlowWidget, "Network Test");
    pWidget->installEventFilter(this);
    pWidget->setGeometry(demoRect);
    demoList.append(pWidget);

    pWidget = new wgTfcard(pictureFlowWidget, "Tfcard Test");
    pWidget->installEventFilter(this);
    pWidget->setGeometry(demoRect);
    demoList.append(pWidget);

    pWidget = new wgUdisk(pictureFlowWidget, "Udisk Test");
    pWidget->installEventFilter(this);
    pWidget->setGeometry(demoRect);
    demoList.append(pWidget);

    pWidget = new wgDeepSleep(pictureFlowWidget, "Deep Sleep Test");
    pWidget->installEventFilter(this);
    pWidget->setGeometry(demoRect);
    demoList.append(pWidget);

    pWidget = new wgReboot(pictureFlowWidget, "Reboot Test");
    pWidget->installEventFilter(this);
    pWidget->setGeometry(demoRect);
    demoList.append(pWidget);

    pWidget = new wgPoweroff(pictureFlowWidget, "Poweroff Test");
    pWidget->installEventFilter(this);
    pWidget->setGeometry(demoRect);
    demoList.append(pWidget);

    return true;
}

bool FluidLauncher::eventFilter(QObject *target, QEvent *event)
{
    if (demoList.contains((DemoWidget*)target))
        {
            if(event->type() == QEvent::LayoutRequest)
            {
                updateImage(0);
                return true;

            }
        }
        return QWidget::eventFilter(target, event);
}

void FluidLauncher::parseSlideshow()
{
	slideShowWidget->addImageDir(":/slides");
}

void FluidLauncher::populatePictureFlow()
{
     pictureFlowWidget->setSlideCount(demoList.count());
 
     for (int i=demoList.count()-1; i>=0; --i) {
         pictureFlowWidget->setSlide(i, *(demoList[i]->getImage()));
         pictureFlowWidget->setSlideCaption(i, demoList[i]->getCaption());
     }
 
     //pictureFlowWidget->setCurrentSlide(demoList.count()/2);
     pictureFlowWidget->setCurrentSlide(0);
     demoList[0]->show();
     demoList[0]->raise();
     demoList[0]->activateWindow();
     demoList[0]->setFocus();
}

void FluidLauncher::updateImage(int index)
{
    Q_UNUSED(index);
    pictureFlowWidget->setSlide(pictureFlowWidget->currentSlide(), *(demoList[pictureFlowWidget->currentSlide()]->getImage()));
}


void FluidLauncher::launchApplication(int index)
{
    Q_UNUSED(index);
    inputTimer->stop();
}

void FluidLauncher::switchToLauncher()
{
    slideShowWidget->stopShow();
    inputTimer->start();
    setCurrentWidget(pictureFlowWidget);
}

void FluidLauncher::resetInputTimeout()
{
    if (inputTimer->isActive())
        inputTimer->start();
}

void FluidLauncher::inputTimedout()
{
    switchToSlideshow();
}

void FluidLauncher::switchToSlideshow()
{
    inputTimer->stop();
    slideShowWidget->startShow();
    setCurrentWidget(slideShowWidget);
}

void FluidLauncher::demoFinished()
{
    setCurrentWidget(pictureFlowWidget);
    inputTimer->start();

    raise();
    activateWindow();
}

void FluidLauncher::changeEvent(QEvent* event)
{
    if (event->type() == QEvent::ActivationChange) {
        if (isActiveWindow()) {
            if(currentWidget() == pictureFlowWidget) {
                resetInputTimeout();
            } else {
                slideShowWidget->startShow();
            }
        } else {
            inputTimer->stop();
            slideShowWidget->stopShow();
        }
    }
    QStackedWidget::changeEvent(event);
}
