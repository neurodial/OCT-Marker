#include <QtGui/QApplication>

#include <windows/octmarkermainwindow.h>

int main(int argc, char **argv)
{
	QApplication app(argc, argv);

/*
	QTranslator translator, translator2;
	translator2.load("prog-" + QLocale::system().name());
	translator.load("/usr/share/qt4/translations/qt_" + QLocale::system().name());
	app.installTranslator(&translator);
	app.installTranslator(&translator2);*/


	OCTMarkerMainWindow octMarkerProg;
	octMarkerProg.show();
	return app.exec();
}
