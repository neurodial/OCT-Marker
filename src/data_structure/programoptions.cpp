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


OptionBool ProgramOptions::fillEmptyPixelWhite(true , "fillEmptyPixelWhite");
OptionBool ProgramOptions::registerBScans     (true , "registerBScans"     );
OptionBool ProgramOptions::loadRotateSlo      (false, "loadRotateSlo"      );

OptionBool ProgramOptions::holdOCTRawData     (false, "holdOCTRawData"     );

OptionInt  ProgramOptions::e2eGrayTransform   (1    , "e2eGrayTransform"   );


OptionBool ProgramOptions::sloShowBscans       (true , "sloShowBscans");
OptionBool ProgramOptions::sloShowOnylActBScan(false, "sloShowOnylActBScan");
OptionBool ProgramOptions::sloShowLabels      (false, "sloShowLabels");


OptionString ProgramOptions::octDirectory      (".", "octDirectory");
OptionString ProgramOptions::loadOctdataAtStart("" , "loadOctDataAtStart");

OptionBool ProgramOptions::autoSaveOctMarkers         (true, "autoSaveOctMarkers");
OptionInt  ProgramOptions::defaultFileformatOctMarkers(static_cast<int>(OctMarkerFileformat::INFO)  , "defaultFileformatOctMarkers");

OptionInt  ProgramOptions::bscanMarkerToolId(-1, "bscanMarkerToolId");
OptionInt  ProgramOptions::  sloMarkerToolId(-1,   "sloMarkerToolId");



void ProgramOptions::resetAllOptions()
{
	std::vector<Option*> options = getAllOptions();
	
	for(Option* opt: options)
		opt->resetValue();
}


QSettings& ProgramOptions::getSettings()
{
	static QSettings settings("becrf", "oct-marker");

	return settings;
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


