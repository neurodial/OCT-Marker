/*
 *
 */

#include "programoptions.h"
#include <QColorDialog>
#include <QSettings>


OptionBool ProgramOptions::fillEmptyPixelWhite(true, "fillEmptyPixelWhite");
OptionBool ProgramOptions::registerBScans     (true, "registerBScans"     );

OptionInt  ProgramOptions::e2eGrayTransform(1, "e2eGrayTransform");


OptionBool ProgramOptions::sloShowBscans(true , "sloShowBscans");
OptionBool ProgramOptions::sloShowLabels(false, "sloShowLabels");



OptionString ProgramOptions::octDirectory(".", "imageDirectory");
OptionString ProgramOptions::loadOctdataAtStart("", "loadImageAtStart");


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
	std::vector<Option*> options = {&fillEmptyPixelWhite, &registerBScans, &e2eGrayTransform, &sloShowBscans, &sloShowLabels, &octDirectory, &loadOctdataAtStart};
	
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


