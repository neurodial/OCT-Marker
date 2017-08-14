/*
 *
 */

#include "programoptions.h"
#include <QColorDialog>
#include <QSettings>


#include <globaldefinitions.h>



OptionBool  ProgramOptions::bscansShowSegmentationslines(true   , "bscansShowSegmentationslines");
OptionColor ProgramOptions::bscanSegmetationLineColor   (Qt::red, "bscanSegmetationLineColor"   );
OptionInt   ProgramOptions::bscanSegmetationLineThicknes(1      , "bscanSegmetationLineThicknes");


OptionBool ProgramOptions::fillEmptyPixelWhite(false, "fillEmptyPixelWhite");
OptionBool ProgramOptions::registerBScans     (true , "registerBScans"     );
OptionBool ProgramOptions::loadRotateSlo      (false, "loadRotateSlo"      );

OptionBool ProgramOptions::holdOCTRawData     (false, "holdOCTRawData"     );
OptionBool ProgramOptions::readBScans         (true , "readBScans"         );

OptionInt  ProgramOptions::e2eGrayTransform   (1    , "e2eGrayTransform"   );


OptionBool ProgramOptions::sloShowBscans      (true , "sloShowBscans");
OptionBool ProgramOptions::sloShowOnylActBScan(true , "sloShowOnylActBScan");
OptionBool ProgramOptions::sloShowLabels      (false, "sloShowLabels");
OptionBool ProgramOptions::sloShowGrid        (true, "sloShowGrid");


OptionString ProgramOptions::octDirectory      (".", "octDirectory");
OptionString ProgramOptions::loadOctdataAtStart("" , "loadOctDataAtStart");

OptionBool ProgramOptions::autoSaveOctMarkers         (true, "autoSaveOctMarkers");
OptionInt  ProgramOptions::defaultFileformatOctMarkers(static_cast<int>(OctMarkerFileformat::INFO)  , "defaultFileformatOctMarkers");

OptionInt  ProgramOptions::bscanMarkerToolId(-1, "bscanMarkerToolId");
OptionInt  ProgramOptions::  sloMarkerToolId(-1,   "sloMarkerToolId");


ProgramOptions::ProgramOptions()
: settings(new QSettings("becrf", "oct-marker"))
{
}

ProgramOptions::~ProgramOptions()
{
	delete settings;
	settings = nullptr; // singelton
}



void ProgramOptions::resetAllOptions()
{
	std::vector<Option*> options = getAllOptions();
	
	for(Option* opt: options)
		opt->resetValue();
}


QSettings& ProgramOptions::getSettings()
{
	return *(getInstance().settings);
}


void ProgramOptions::readAllOptions()
{
	QSettings& settings = getSettings();
	std::vector<Option*> options = ProgramOptions::getAllOptions();
	
	settings.beginGroup("ProgramOptions");
	for(Option* opt : options)
		opt->setVariant(settings.value(opt->getName(), opt->getVariant()));
	settings.endGroup();
}


void ProgramOptions::writeAllOptions()
{
	QSettings& settings = getSettings();
	std::vector<Option*> options = ProgramOptions::getAllOptions();
	
	settings.beginGroup("ProgramOptions");
	for(Option* opt : options)
		settings.setValue(opt->getName(), opt->getVariant());
	settings.endGroup();
}


std::vector<Option*>& ProgramOptions::getAllOptionsPrivate()
{
	static std::vector<Option*> options;

	return options;
}



void ProgramOptions::registerOption(Option* option)
{
	getAllOptionsPrivate().push_back(option);
}


void ProgramOptions::setIniFile(const QString& iniFilename)
{
	if(!iniFilename.isEmpty())
	{
		ProgramOptions& instance = getInstance();
		QSettings* oldSettings = instance.settings;
		instance.settings = new QSettings(iniFilename, QSettings::IniFormat);
		delete oldSettings;
	}
}
