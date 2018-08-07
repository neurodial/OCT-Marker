/*
 *
 */

#include "programoptions.h"
#include <QColorDialog>
#include <QSettings>

#include <map>


#include <globaldefinitions.h>



OptionBool   ProgramOptions::bscansShowSegmentationslines(true   , "bscansShowSegmentationslines", "ProgramOptions");
OptionColor  ProgramOptions::bscanSegmetationLineColor   (Qt::red, "bscanSegmetationLineColor"   , "ProgramOptions");
OptionInt    ProgramOptions::bscanSegmetationLineThicknes(1      , "bscanSegmetationLineThicknes", "ProgramOptions");
OptionInt    ProgramOptions::bscanAspectRatioType        (0      , "bscanAspectRatioType"        , "ProgramOptions"); // 0 fix value, 1 from bscan, 2 best fit
OptionBool   ProgramOptions::bscanAutoFitImage           (true   , "bscanAutoFitImage"           , "ProgramOptions");

OptionBool   ProgramOptions::bscanShowExtraSegmentationslines(true, "bscanShowExtraSegmentationslines", "ExtraData");


OptionBool   ProgramOptions::fillEmptyPixelWhite(false, "fillEmptyPixelWhite", "ProgramOptions");
OptionBool   ProgramOptions::registerBScans     (true , "registerBScans"     , "ProgramOptions");
OptionBool   ProgramOptions::loadRotateSlo      (false, "loadRotateSlo"      , "ProgramOptions");

OptionBool   ProgramOptions::saveOctBinFlat     (false, "saveOctBinFlat"     , "ProgramOptions");

OptionBool   ProgramOptions::holdOCTRawData     (false, "holdOCTRawData"     , "ProgramOptions");
OptionBool   ProgramOptions::readBScans         (true , "readBScans"         , "ProgramOptions");

OptionInt    ProgramOptions::e2eGrayTransform   (1    , "e2eGrayTransform"   , "ProgramOptions");


OptionBool   ProgramOptions::sloShowLabels       (false, "sloShowLabels"       , "ProgramOptions");
OptionBool   ProgramOptions::sloShowGrid         (true , "sloShowGrid"         , "ProgramOptions");
OptionBool   ProgramOptions::sloShowBScanMousePos(true , "sloShowBScanMousePos", "ProgramOptions");

OptionInt    ProgramOptions::sloShowsBScansPos (1    , "sloShowsBScansPos"   , "ProgramOptions"); // 0 nothing, 1 act BScan, 2 all BScans
OptionBool   ProgramOptions::sloShowOverlay    (true , "sloShowOverlay"      , "ProgramOptions");
OptionDouble ProgramOptions::sloOverlayAlpha   (0.7  , "sloOverlayAlpha"     , "ProgramOptions");
OptionBool   ProgramOptions::sloClipScanArea   (false, "sloClipScanArea"      , "ProgramOptions");


OptionString ProgramOptions::octDirectory      (".", "octDirectory"      , "ProgramOptions");
OptionString ProgramOptions::loadOctdataAtStart("" , "loadOctDataAtStart", "ProgramOptions");

OptionBool   ProgramOptions::autoSaveOctMarkers         (true, "autoSaveOctMarkers", "ProgramOptions");
OptionInt    ProgramOptions::defaultFileformatOctMarkers(static_cast<int>(OctMarkerFileformat::INFO), "defaultFileformatOctMarkers", "ProgramOptions");

OptionInt    ProgramOptions::bscanMarkerToolId(-1, "bscanMarkerToolId", "ProgramOptions");
OptionInt    ProgramOptions::  sloMarkerToolId(-1,   "sloMarkerToolId", "ProgramOptions");

OptionDouble ProgramOptions::layerSegFindPointInsertTol  (0.2 , "PointInsertTol"  , "LayerSeg", 0, 10, 0.1);
OptionDouble ProgramOptions::layerSegFindPointRemoveTol  (0.1 , "PointRemoveTol"  , "LayerSeg", 0, 10, 0.05);
OptionDouble ProgramOptions::layerSegFindPointMaxAbsError(0.25, "PointMaxAbsError", "LayerSeg", 0, 10, 0.1);
OptionInt    ProgramOptions::layerSegFindPointMaxPoints  (0   , "PointMaxPoints"  , "LayerSeg", 0, 1000);


OptionColor  ProgramOptions::layerSegActiveLineColor     (Qt::red   , "ActiveLineColor"  , "LayerSeg");
OptionColor  ProgramOptions::layerSegPassivLineColor     (Qt::yellow, "PassivLineColor"  , "LayerSeg");
OptionInt    ProgramOptions::layerSegActiveLineSize      (2         , "ActiveLineSize"   , "LayerSeg", 1, 15);
OptionInt    ProgramOptions::layerSegPassivLineSize      (1         , "PassivLineSize"   , "LayerSeg", 1, 15);

OptionInt    ProgramOptions::layerSegSplinePointSize     (8         , "SplinePointSize"  , "LayerSeg", 5, 30);
OptionBool   ProgramOptions::layerSegThicknessmapBlend   (true      , "ThicknessmapBlend", "LayerSeg");


OptionInt    ProgramOptions::freeFormedSegmetationLineThickness(1      , "bscanSegmetationLineThickness", "FreeFormedSegmentation", 1, 10);
OptionBool   ProgramOptions::freeFormedSegmetationShowArea     (true   , "showArea"                     , "FreeFormedSegmentation");


OptionBool   ProgramOptions::intervallMarkSloMapAuteGenerate(false    , "SloMapAuteGenerate", "IntervallMark");



namespace
{
	QSettings* generateSettings(const QString& postfix = QString())
	{
		return new QSettings("becrf", "oct-marker" + postfix);
	}
}

ProgramOptions::ProgramOptions()
: settings(generateSettings())
, resetAction(new QAction)
{
	resetAction->setText(tr("reset configuration"));
	resetAction->setToolTip(tr("set all options to default"));
	resetAction->setIcon(QIcon(":/icons/tango/actions/edit-clear.svgz"));
	connect(resetAction, &QAction::triggered, this, &ProgramOptions::resetAllOptionsPrivate);
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
	if(!getSaveOptions())
		return;

	QSettings& settings = getSettings();

	ConfigList& list = getAllOptionsPrivate();
	for(auto optClass : list.sortedConfig)
	{
		std::vector<Option*> options = optClass.second;

		settings.beginGroup(optClass.first);
		for(Option* opt : options)
		{
			if(opt->isDefault())
				settings.remove(opt->getName());
			else
				settings.setValue(opt->getName(), opt->getVariant());
		}
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

void ProgramOptions::setOptionsPostfix(const QString& postfix)
{
	ProgramOptions& instance = getInstance();
	QSettings* oldSettings = instance.settings;
	instance.settings = generateSettings(postfix);
	delete oldSettings;
}

void ProgramOptions::setSaveOptions(bool save)
{
	getInstance().saveOptions = save;
}

bool ProgramOptions::getSaveOptions()
{
	return getInstance().saveOptions;
}



QAction* ProgramOptions::getResetAction()
{
	return getInstance().resetAction;
}
