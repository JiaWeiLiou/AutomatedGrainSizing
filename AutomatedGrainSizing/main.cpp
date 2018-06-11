#include "showmainwindow.h"
#include <QtWidgets/QApplication>
#include <QPixmap>
#include <QSplashScreen>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	QPixmap pixmap("SplashScreen.png");
	QSplashScreen splash(pixmap);
	splash.show();
	a.processEvents();
	ShowMainWindow w;
	w.show();
	splash.finish(&w);
	return a.exec();
}
