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

#include "programoptionstype.h"
#include<QColorDialog>
#include<QInputDialog>
#include<QLineEdit>
#include<QSpinBox>

#include<data_structure/programoptions.h>
#include<widgets/doubleslider.h>


Option::Option(const QString& name, const QString& optClass)
: name    (name    )
, optClass(optClass)
{
	ProgramOptions::registerOption(this);
}


void OptionBool::setDescriptions(const QString& shortDesc, const QString& longDesc)
{
	Option::setDescriptions(shortDesc, longDesc);

	action->setText(shortDesc);
	action->setToolTip(longDesc);
}


OptionDouble::OptionDouble(const double v, const QString& name, const QString& optClass, double min, double max, double stepSize)
: Option(name, optClass)
, value(v)
, defaultValue(v)
, valueMin(min)
, valueMax(max)
, valueStepSize(stepSize)
, inputDialogAction(new QAction(this))
{
	connect(inputDialogAction, &QAction::triggered, this, &OptionDouble::showInputDialog);
}

QSlider* OptionDouble::createSlider(Qt::Orientation orientation, QWidget* parent)
{
	DoubleSlider* dslider = new DoubleSlider(orientation, static_cast<int>((valueMax-valueMin)*200), nullptr);
	dslider->setRange(valueMin, valueMax);
	dslider->setValue(value);

	connect(dslider, &DoubleSlider::valueChanged, this, &OptionDouble::setValue);
	connect(this, &OptionDouble::valueChanged, dslider, &DoubleSlider::setValue);
	return dslider;
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
	int oldValue = value;
	QInputDialog dialog;
	dialog.setInputMode(QInputDialog::IntInput);
	dialog.setIntRange(valueMin, valueMax);
	dialog.setIntValue(value);
	dialog.setIntStep(valueStepSize);
	dialog.setWindowTitle(getDescriptionShort());
	dialog.setLabelText(getDescription() + "\n" + tr("Default value: %1").arg(defaultValue));

	connect(&dialog, &QInputDialog::intValueChanged, this, &OptionInt::setValue);

	int result = dialog.exec();
	if(result != QDialog::Accepted)
		setValue(oldValue);
}

QSpinBox* OptionInt::createSpinBox(QWidget* parent)
{
	QSpinBox* spinbox = new QSpinBox(parent);
	spinbox->setRange(valueMin, valueMax);
	spinbox->setValue(value);
	spinbox->setSingleStep(valueStepSize);
	spinbox->setToolTip(getDescription());

	connect(spinbox, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), this   , &OptionInt::setValue);
	connect(this   , &OptionInt::valueChanged                                    , spinbox, &QSpinBox ::setValue);

	return spinbox;
}


void OptionInt::setDescriptions(const QString& shortDesc, const QString& longDesc)
{
	Option::setDescriptions(shortDesc, longDesc);

	inputDialogAction->setText(shortDesc);
	inputDialogAction->setToolTip(longDesc);
}

void OptionDouble::showInputDialog()
{
	double oldValue = value;
	QInputDialog dialog;
	dialog.setInputMode(QInputDialog::DoubleInput);
	dialog.setDoubleRange(valueMin, valueMax);
	dialog.setDoubleValue(value);
#if QT_VERSION >= QT_VERSION_CHECK(5, 10, 0)
	dialog.setDoubleStep(valueStepSize);
#endif
	dialog.setWindowTitle(getDescriptionShort());
	dialog.setLabelText(getDescription() + "\n" + tr("Default value: %1").arg(defaultValue));

	connect(&dialog, &QInputDialog::doubleValueChanged, this, &OptionDouble::setValue);

	int result = dialog.exec();
	if(result != QDialog::Accepted)
		setValue(oldValue);
}

void OptionDouble::setDescriptions(const QString& shortDesc, const QString& longDesc)
{
	Option::setDescriptions(shortDesc, longDesc);

	inputDialogAction->setText(shortDesc);
	inputDialogAction->setToolTip(longDesc);
}
