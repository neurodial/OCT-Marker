#include <QApplication>

#include <QTranslator>
#include <QLocale>
#include <QFile>

#include <windows/octmarkermainwindow.h>
#include "data_structure/programoptions.h"

#include <buildconstants.h>

#include <iostream>

#include <QDir>
#include <QFileInfo>


bool loadMarkerTranslatorFile(QTranslator& translator, const QString& dir)
{
	return translator.load(QLocale::system(), "octmarker", "-", dir, ".qm");
}

void loadMarkerTranslator(QTranslator& translator, const QString& programDir)
{
	if(loadMarkerTranslatorFile(translator, programDir)) return;
	if(loadMarkerTranslatorFile(translator, "/usr/share/octmarker/locale/")) return;
	loadMarkerTranslatorFile(translator, "");
}

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

	QDir d = QFileInfo(argv[0]).absoluteDir();
	loadMarkerTranslator(translator2, d.absolutePath());

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
