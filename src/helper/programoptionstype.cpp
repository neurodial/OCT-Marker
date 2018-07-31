#include "programoptionstype.h"
#include<QColorDialog>
#include<QInputDialog>
#include<QLineEdit>

#include <data_structure/programoptions.h>


Option::Option(const QString& name, const QString& optClass)
: name    (name    )
, optClass(optClass)
{
	ProgramOptions::registerOption(this);
}

void OptionColor::showColorDialog()
{
	QColor oldColor = value;
	bool colorChangd = false;
	QColorDialog dialog;
	dialog.setCurrentColor(value);
	connect(&dialog, &QColorDialog::currentColorChanged, this, &OptionColor::setValue);
	if(dialog.exec())
	{
		value = dialog.currentColor();
		colorChangd = (value != oldColor);
	}
	else
	{
		colorChangd = (value != oldColor);
		value = oldColor;
	}

	if(colorChangd)
		emit(valueChanged(value));
}

void OptionColor::setDescriptions(const QString& shortDesc, const QString& longDesc)
{
	Option::setDescriptions(shortDesc, longDesc);

	colorDialogAction->setText(shortDesc);
	colorDialogAction->setToolTip(longDesc);
}


void OptionInt::showInputDialog()
{
	bool ok;
	int v = QInputDialog::getInt(nullptr, getDescriptionShort(), getDescription(), value, valueMin, valueMax, 1, &ok);
	if(ok)
		setValue(v);
}

void OptionInt::setDescriptions(const QString& shortDesc, const QString& longDesc)
{
	Option::setDescriptions(shortDesc, longDesc);

	inputDialogAction->setText(shortDesc);
	inputDialogAction->setToolTip(longDesc);
}

void OptionDouble::showInputDialog()
{
	bool ok;
	double v = QInputDialog::getDouble(nullptr, getDescriptionShort(), getDescription(), value, valueMin, valueMax, 1, &ok);
	if(ok)
		setValue(v);
}

void OptionDouble::setDescriptions(const QString& shortDesc, const QString& longDesc)
{
	Option::setDescriptions(shortDesc, longDesc);

	inputDialogAction->setText(shortDesc);
	inputDialogAction->setToolTip(longDesc);
}
