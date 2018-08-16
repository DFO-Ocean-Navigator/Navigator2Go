#include "mainwindow.h"
#include <QApplication>

int main(int argc, char* argv[]) {

	std::set_terminate([](){ qFatal("An unhandled exception has occured. Please contact nabil.miri@dfo-mpo.gc.ca for technical support."); });

	const QApplication a(argc, argv);

	MainWindow w;
	w.show();

	return a.exec();
}
