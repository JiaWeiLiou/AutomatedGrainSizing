#include "showmainwindow.h"
#include <QtWidgets/QApplication>
#include <QPixmap>
#include <QSplashScreen>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	QSplashScreen splash(QPixmap(":/Resources/SplashScreen.png"));
	splash.show();
	a.processEvents();
	ShowMainWindow w;
	w.show();
	splash.finish(&w);
	return a.exec();
}
