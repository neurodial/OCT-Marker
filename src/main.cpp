#include <QApplication>

 #include <QTranslator>
#include <QLocale>
#include <QFile>

#include <windows/octmarkermainwindow.h>
#include "data_structure/programoptions.h"

#include <buildconstants.h>

int main(int argc, char **argv)
{
	QApplication app(argc, argv);

	qDebug("Build Type      : %s", BuildConstants::buildTyp);
	qDebug("Git Hash        : %s", BuildConstants::gitSha1);
	qDebug("Build Date      : %s", BuildConstants::buildDate);
	qDebug("Build Time      : %s", BuildConstants::buildTime);
	qDebug("Compiler Id     : %s", BuildConstants::compilerId);
	qDebug("Compiler Version: %s", BuildConstants::compilerVersion);

	QTranslator translator, translator2;
	// translator.load("/usr/share/qt4/translations/qt_" + QLocale::system().name());
	translator.load(QLocale::system(), "qt", "_", "/usr/share/qt5/translations", ".qm");

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


	ProgramOptions::readAllOptions();

	const char* filename = nullptr;
	if(argc > 1)
		filename = argv[argc-1];

	OCTMarkerMainWindow octMarkerProg(filename);
	octMarkerProg.show();
	return app.exec();
}
