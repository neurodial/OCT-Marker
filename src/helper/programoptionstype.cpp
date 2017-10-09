#include "programoptionstype.h"
#include <QColorDialog>

#include <data_structure/programoptions.h>


Option::Option(const QString& name, const QString& optClass)
: name    (name    )
, optClass(optClass)
{
	ProgramOptions::registerOption(this);
}

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

