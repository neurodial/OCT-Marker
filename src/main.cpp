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
#include <qlibraryinfo.h>


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

bool loadQtTranslatorFile(QTranslator& translator, const QString& dir)
{
	return translator.load(QLocale::system(), "qt", "_", dir, ".qm");
}

void loadQtTranslator(QTranslator& translator, const QString& programDir)
{
	QString translationsPath(QLibraryInfo::location(QLibraryInfo::TranslationsPath));
	if(loadQtTranslatorFile(translator, programDir)) return;
	if(loadQtTranslatorFile(translator, translationsPath)) return;
	if(loadQtTranslatorFile(translator, "/usr/share/qt5/translations")) return;
	loadQtTranslatorFile(translator, "");
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

	QTranslator translator;

	QTranslator qtTranslator;


	QDir d = QFileInfo(argv[0]).absoluteDir();
	loadQtTranslator(qtTranslator, d.absolutePath());
	loadMarkerTranslator(translator, d.absolutePath());

	app.installTranslator(&qtTranslator);
	app.installTranslator(&translator);


	ProgramOptions::readAllOptions();

	const char* filename = nullptr;
	if(argc > 1)
		filename = argv[argc-1];

	OCTMarkerMainWindow octMarkerProg(filename);
	octMarkerProg.show();
	return app.exec();
}
