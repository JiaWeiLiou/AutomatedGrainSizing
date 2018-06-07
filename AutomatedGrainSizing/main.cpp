#include "showmainwindow.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	ShowMainWindow w;
	w.show();
	return a.exec();
}
