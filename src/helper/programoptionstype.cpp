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
	QString labelText = getDescription() + "\n" + tr("Default value: %1").arg(defaultValue);
	bool ok;
	int v = QInputDialog::getInt(nullptr, getDescriptionShort(), labelText, value, valueMin, valueMax, valueStepSize, &ok);
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
	QInputDialog dialog;
	dialog.setInputMode(QInputDialog::DoubleInput);
	dialog.setDoubleRange(valueMin, valueMax);
	dialog.setDoubleValue(value);
#if QT_VERSION >= QT_VERSION_CHECK(5, 10, 0)
	dialog.setDoubleStep(valueStepSize);
#endif
	dialog.setWindowTitle(getDescriptionShort());
	dialog.setLabelText(getDescription() + "\n" + tr("Default value: %1").arg(defaultValue));

	int result = dialog.exec();
	if(result == QDialog::Accepted)
		setValue(dialog.doubleValue());
}

void OptionDouble::setDescriptions(const QString& shortDesc, const QString& longDesc)
{
	Option::setDescriptions(shortDesc, longDesc);

	inputDialogAction->setText(shortDesc);
	inputDialogAction->setToolTip(longDesc);
}
