/*
 *
 */

#include "programoptions.h"
#include <QColorDialog>
#include <QSettings>

#include <map>


#include <globaldefinitions.h>



OptionBool  ProgramOptions::bscansShowSegmentationslines(true   , "bscansShowSegmentationslines", "ProgramOptions");
OptionColor ProgramOptions::bscanSegmetationLineColor   (Qt::red, "bscanSegmetationLineColor"   , "ProgramOptions");
OptionInt   ProgramOptions::bscanSegmetationLineThicknes(1      , "bscanSegmetationLineThicknes", "ProgramOptions");
OptionBool  ProgramOptions::bscanRespectAspectRatio     (false  , "bscanRespectAspectRatio"     , "ProgramOptions");

OptionBool  ProgramOptions::bscanShowExtraSegmentationslines(true, "bscanShowExtraSegmentationslines", "ExtraData");


OptionBool ProgramOptions::fillEmptyPixelWhite(false, "fillEmptyPixelWhite", "ProgramOptions");
OptionBool ProgramOptions::registerBScans     (true , "registerBScans"     , "ProgramOptions");
OptionBool ProgramOptions::loadRotateSlo      (false, "loadRotateSlo"      , "ProgramOptions");

OptionBool ProgramOptions::saveOctBinFlat     (false, "saveOctBinFlat"     , "ProgramOptions");

OptionBool ProgramOptions::holdOCTRawData     (false, "holdOCTRawData"     , "ProgramOptions");
OptionBool ProgramOptions::readBScans         (true , "readBScans"         , "ProgramOptions");

OptionInt  ProgramOptions::e2eGrayTransform   (1    , "e2eGrayTransform"   , "ProgramOptions");


OptionBool ProgramOptions::sloShowLabels       (false, "sloShowLabels"       , "ProgramOptions");
OptionBool ProgramOptions::sloShowGrid         (true , "sloShowGrid"         , "ProgramOptions");
OptionBool ProgramOptions::sloShowBScanMousePos(true , "sloShowBScanMousePos", "ProgramOptions");

OptionInt  ProgramOptions::sloShowsBScansPos   (1    , "sloShowsBScansPos"   , "ProgramOptions"); // 0 nothing, 1 act BScan, 2 all BScans
OptionBool   ProgramOptions::sloShowOverlay    (true , "sloShowOverlay"      , "ProgramOptions");
OptionDouble ProgramOptions::sloOverlayAlpha   (0.7  , "sloOverlayAlpha"     , "ProgramOptions");


OptionString ProgramOptions::octDirectory      (".", "octDirectory"      , "ProgramOptions");
OptionString ProgramOptions::loadOctdataAtStart("" , "loadOctDataAtStart", "ProgramOptions");

OptionBool ProgramOptions::autoSaveOctMarkers         (true, "autoSaveOctMarkers", "ProgramOptions");
OptionInt  ProgramOptions::defaultFileformatOctMarkers(static_cast<int>(OctMarkerFileformat::INFO)  , "defaultFileformatOctMarkers", "ProgramOptions");

OptionInt  ProgramOptions::bscanMarkerToolId(-1, "bscanMarkerToolId", "ProgramOptions");
OptionInt  ProgramOptions::  sloMarkerToolId(-1,   "sloMarkerToolId", "ProgramOptions");

OptionDouble ProgramOptions::layerSegFindPointInsertTol  (0.2 , "PointInsertTol"  , "LayerSeg");
OptionDouble ProgramOptions::layerSegFindPointRemoveTol  (0.1 , "PointRemoveTol"  , "LayerSeg");
OptionDouble ProgramOptions::layerSegFindPointMaxAbsError(0.25, "PointMaxAbsError", "LayerSeg");


OptionColor  ProgramOptions::layerSegActiveLineColor     (Qt::red , "ActiveLineColor", "LayerSeg");
OptionColor  ProgramOptions::layerSegPassivLineColor     (Qt::blue, "PassivLineColor", "LayerSeg");
OptionInt    ProgramOptions::layerSegActiveLineSize      (       2, "ActiveLineSize" , "LayerSeg");
OptionInt    ProgramOptions::layerSegPassivLineSize      (       1, "PassivLineSize" , "LayerSeg");

OptionInt    ProgramOptions::layerSegSplinePointSize     (       8, "SplinePointSize", "LayerSeg");
OptionBool   ProgramOptions::layerSegThicknessmapBlend   (true    , "ThicknessmapBlend", "LayerSeg");


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

	ConfigList& list = getAllOptionsPrivate();
	for(auto optClass : list.sortedConfig)
	{
		std::vector<Option*> options = optClass.second;

		settings.beginGroup(optClass.first);
		for(Option* opt : options)
			opt->setVariant(settings.value(opt->getName(), opt->getVariant()));
		settings.endGroup();
	}
}


void ProgramOptions::writeAllOptions()
{
	QSettings& settings = getSettings();


	ConfigList& list = getAllOptionsPrivate();
	for(auto optClass : list.sortedConfig)
	{
		std::vector<Option*> options = optClass.second;

		settings.beginGroup(optClass.first);
		for(Option* opt : options)
			settings.setValue(opt->getName(), opt->getVariant());
		settings.endGroup();
	}
}


ProgramOptions::ConfigList& ProgramOptions::getAllOptionsPrivate()
{
	static ConfigList options;

	return options;
}



void ProgramOptions::registerOption(Option* option)
{
	ConfigList& list = getAllOptionsPrivate();
	list.allConfig.push_back(option);
	list.sortedConfig[option->getClass()].push_back(option);
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
