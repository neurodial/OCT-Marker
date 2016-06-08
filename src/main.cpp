#include <QApplication>

 #include <QTranslator>
#include <QLocale>
#include <QFile>

#include <windows/octmarkermainwindow.h>

int main(int argc, char **argv)
{
	QApplication app(argc, argv);


	QTranslator translator, translator2;
	// translator.load("/usr/share/qt4/translations/qt_" + QLocale::system().name());
	translator.load(QLocale::system(), "qt", "_", "/usr/share/qt4/translations", ".qm");

	QFile file("octmarker-de.qm");
	if(file.exists())
	{
		qDebug("Load local translation %s", file.fileName().toStdString().c_str());
		translator2.load(file.fileName());
	}
	else
		translator2.load(QLocale::system(), "octmarker", "-", "/usr/share/octmarker/locale/", ".qm");
	app.installTranslator(&translator);
	app.installTranslator(&translator2);



	OCTMarkerMainWindow octMarkerProg;
	octMarkerProg.show();
	return app.exec();
}
