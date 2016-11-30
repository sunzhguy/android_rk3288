#include <QApplication>
#include <QStringList>

#include "fluidlauncher.h"


int main(int argc, char** argv)
{
	QStringList originalArgs;

	for (int i=0; i<argc; i++)
		originalArgs << argv[i];

	QApplication app(argc, argv);

    QFile file(":/qss/coffee.qss");
    file.open(QFile::ReadOnly);
    QString styleSheet = QLatin1String(file.readAll());
    qApp->setStyleSheet(styleSheet);
	FluidLauncher launcher(&originalArgs);
	launcher.switchToPictureFlow();


	return app.exec();
}
