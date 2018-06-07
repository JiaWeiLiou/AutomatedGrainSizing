#include "AutomaticGrainSizing.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	AutomaticGrainSizing w;
	w.show();
	return a.exec();
}
