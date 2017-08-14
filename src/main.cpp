#include <QApplication>

#include <QTranslator>
#include <QLocale>
#include <QFile>

#include <windows/octmarkermainwindow.h>
#include <windows/stupidsplinewindow.h>
#include "data_structure/programoptions.h"

#include <buildconstants.h>

#include <iostream>

#include <QDir>
#include <QFileInfo>
#include <qlibraryinfo.h>
#include <QCommandLineParser>


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
	QCoreApplication::setApplicationName("OCT-Marker");
	QCoreApplication::setApplicationVersion(BuildConstants::gitSha1);


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


	QCommandLineParser parser;
    parser.setApplicationDescription("Ein Programm um verschiedene Dinge in OCT-Daten zu markieren und zu segmentieren.");
	parser.addOptions({
		// A boolean option with a single name (-p)
		{"i-want-stupid-spline-gui", QCoreApplication::translate("main", "Show stupid spline gui")},
		// A boolean option with multiple names (-f, --force)
		{{"f", "force"}, QCoreApplication::translate("main", "Overwrite existing files.")},
		// An option with a value
		{{"t", "target-directory"},
		    QCoreApplication::translate("main", "Copy all source files into <directory>."),
		    QCoreApplication::translate("main", "directory")},
	});

	parser.addHelpOption();
	parser.addVersionOption();


	// Process the actual command line arguments given by the user
	parser.process(app);


	const char* filename = nullptr;
	if(argc > 1)
		filename = argv[argc-1];

	bool stupidSplineGui = parser.isSet("i-want-stupid-spline-gui");
	if(stupidSplineGui)
	{
		StupidSplineWindow octMarkerProg(filename);
		octMarkerProg.show();
		return app.exec();
	}
	else
	{
		ProgramOptions::readAllOptions();
		OCTMarkerMainWindow octMarkerProg(filename);
		octMarkerProg.show();
		return app.exec();
	}
}
