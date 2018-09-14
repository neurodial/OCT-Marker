/*
 * Copyright (c) 2018 Kay Gawlik <kaydev@amarunet.de> <kay.gawlik@beuth-hochschule.de> <kay.gawlik@charite.de>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include <QApplication>

#include <iostream>

#include <QTranslator>
#include <QLocale>
#include <QFile>
#include <QDir>
#include <QFileInfo>
#include <qlibraryinfo.h>
#include <QCommandLineParser>

#include <windows/octmarkermainwindow.h>
#include <windows/stupidsplinewindow.h>
#include "data_structure/programoptions.h"

#include <buildconstants.h>

#include "prepareprogrammoptions.h"


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
// 	QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

	QApplication app(argc, argv);
	QCoreApplication::setApplicationName("OCT-Marker");
	QCoreApplication::setApplicationVersion(BuildConstants::gitSha1);

	QTranslator translator;

	QTranslator qtTranslator;


	QDir d = QFileInfo(argv[0]).absoluteDir();
	loadQtTranslator(qtTranslator, d.absolutePath());
	loadMarkerTranslator(translator, d.absolutePath());

	app.installTranslator(&qtTranslator);
	app.installTranslator(&translator);


	QCommandLineParser parser;
    parser.setApplicationDescription("Ein Programm um verschiedene Dinge in OCT-Daten zu markieren und zu segmentieren.\n\nOCT-Marker  Copyright (C) 2018 Kay Gawlik\nThis program comes with ABSOLUTELY NO WARRANTY. This is free software, and you are welcome to redistribute it under certain conditions; for details type ‘--license’.");
	parser.addOptions({
		// A boolean option with a single name (-p)
		{"license"                 , QCoreApplication::translate("main", "Show license text")},
		{"i-want-stupid-spline-gui", QCoreApplication::translate("main", "Show stupid spline gui")},
		{"dont-save-options"       , QCoreApplication::translate("main", "dont save options set in application")},
		{{"i", "ini-file"},
		    QCoreApplication::translate("main", "use config from ini file"),
		    QCoreApplication::translate("main", "ini file")},
	});

	parser.addHelpOption();
	parser.addVersionOption();

	parser.addPositionalArgument("file", QCoreApplication::translate("main", "load this oct file at start"), "[file]");

	// Process the actual command line arguments given by the user
	parser.process(app);

	if(parser.isSet("license"))
	{
		QFile file(":/license.txt");
		if(file.open(QIODevice::ReadOnly))
			std::cout << file.readAll().toStdString();
		else
			std::cerr << "Error: Missing license file.\n";
		return 0;
	}

	if(parser.isSet("ini-file"))
	{
		QString iniFile = parser.value("ini-file");
		ProgramOptions::setIniFile(iniFile);
	}

	if(parser.isSet("dont-save-options"))
		ProgramOptions::setSaveOptions(false);

    const QStringList fileList = parser.positionalArguments();

	bool gitTimeOk = true;
	QString gitTimeStr(BuildConstants::gitTimeStamp);
	QDateTime gitTime = QDateTime::fromSecsSinceEpoch(gitTimeStr.toLongLong(&gitTimeOk));

	qDebug("Build Type      : %s", BuildConstants::buildTyp);
	qDebug("Git Hash        : %s", BuildConstants::gitSha1);
	if(gitTimeOk) qDebug("Git Date        : %s", gitTime.toString(Qt::ISODate).toUtf8().data());
	qDebug("Compiler Id     : %s", BuildConstants::compilerId);
	qDebug("Compiler Version: %s", BuildConstants::compilerVersion);


	QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

	PrepareProgrammOptions::prepareProgrammOptions();

	bool stupidSplineGui = parser.isSet("i-want-stupid-spline-gui");
	if(stupidSplineGui)
	{
		if(!parser.isSet("ini-file"))
			ProgramOptions::setOptionsPostfix("-spline-gui");
		ProgramOptions::readAllOptions();

		StupidSplineWindow octMarkerProg;
		if(fileList.size() > 0)
			octMarkerProg.loadFile(fileList.at(0));
		octMarkerProg.show();
		return app.exec();
	}
	else
	{
		ProgramOptions::readAllOptions();
		bool loadFile = fileList.size() > 0;
		OCTMarkerMainWindow octMarkerProg(!loadFile);
		if(loadFile)
			octMarkerProg.loadFile(fileList.at(0));
		octMarkerProg.show();
		return app.exec();
	}
}
