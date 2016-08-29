/*
 *
 */

#include "programoptions.h"
#include <QColorDialog>
#include <QSettings>


OptionBool ProgramOptions::fillEmptyPixelWhite(true, "fillEmptyPixelWhite");
OptionBool ProgramOptions::registerBScanns(true, "registerBScanns");

OptionInt  ProgramOptions::e2eGrayTransform(1, "e2eGrayTransform");



OptionString ProgramOptions::imageDirectory("../../../material/XML_QMROCT-noCorrection/", "imageDirectory");
OptionString ProgramOptions::loadImageAtStart("../../../material/XML_QMROCT-noCorrection/79C551C0.tif", "loadImageAtStart");


void OptionColor::showColorDialog()
{
	QColorDialog dialog;
	dialog.setCurrentColor(value);
	if(dialog.exec())
	{
		value = dialog.currentColor();
		emit(valueChanged(value));
	}
}


std::vector<Option*> ProgramOptions::getAllOptions()
{
	std::vector<Option*> options = {&fillEmptyPixelWhite, &registerBScanns, &e2eGrayTransform, &imageDirectory, &loadImageAtStart};
	
	return options;
}

void ProgramOptions::resetAllOptions()
{
	std::vector<Option*> options = getAllOptions();
	
	for(Option* opt: options)
		opt->resetValue();
}




void ProgramOptions::readAllOptions()
{
	QSettings settings("becrf", "oct-marker");
	std::vector<Option*> options = ProgramOptions::getAllOptions();
	
	settings.beginGroup("ProgramOptions");
	for(Option* opt : options)
		opt->setVariant(settings.value(opt->getName(), opt->getVariant()));
	settings.endGroup();
}


void ProgramOptions::writeAllOptions()
{
	QSettings settings("becrf", "oct-marker");
	std::vector<Option*> options = ProgramOptions::getAllOptions();
	
	settings.beginGroup("ProgramOptions");
	for(Option* opt : options)
		settings.setValue(opt->getName(), opt->getVariant());
	settings.endGroup();
}


