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

#include "dwimagecoloradjustments.h"

#include <QSlider>

#include <imagefilter/filtergammacontrastbrightness.h>
#include <QFormLayout>

DWImageColorAdjustments::DWImageColorAdjustments(QWidget* parent)
: QDockWidget(parent)
, imageFilter(new FilterGammaContrastBrightness)
{
	setWindowTitle(tr("image color adjustments"));

	QWidget* widget  = new QWidget(this);
	gammaSlider      = new QSlider(Qt::Horizontal, this);
	contrastSlider   = new QSlider(Qt::Horizontal, this);
	brightnessSlider = new QSlider(Qt::Horizontal, this);

	connect(gammaSlider     , &QSlider::valueChanged, this, &DWImageColorAdjustments::parameterChanged);
	connect(contrastSlider  , &QSlider::valueChanged, this, &DWImageColorAdjustments::parameterChanged);
	connect(brightnessSlider, &QSlider::valueChanged, this, &DWImageColorAdjustments::parameterChanged);


	gammaSlider     ->setMinimum(   0);
	gammaSlider     ->setMaximum( 200);
	gammaSlider     ->setValue  ( 100);
	contrastSlider  ->setMinimum(   0);
	contrastSlider  ->setMaximum( 200);
	contrastSlider  ->setValue  ( 100);
	brightnessSlider->setMinimum(-100);
	brightnessSlider->setMaximum( 100);

	gammaSlider     ->setTickPosition(QSlider::TicksBelow);
	gammaSlider     ->setTickInterval(20);
	contrastSlider  ->setTickPosition(QSlider::TicksBelow);
	contrastSlider  ->setTickInterval(20);
	brightnessSlider->setTickPosition(QSlider::TicksBelow);
	brightnessSlider->setTickInterval(20);

	QFormLayout *formLayout = new QFormLayout;
	formLayout->addRow(tr("Gamma:"     ), gammaSlider     );
	formLayout->addRow(tr("Contrast:"  ), contrastSlider  );
	formLayout->addRow(tr("Brightness:"), brightnessSlider);
	widget->setLayout(formLayout);

	parameterChanged();
	setWidget(widget);
}

DWImageColorAdjustments::~DWImageColorAdjustments()
{
	delete imageFilter;
}


const FilterImage* DWImageColorAdjustments::getImageFilter() const
{
	return imageFilter;
}


void DWImageColorAdjustments::parameterChanged()
{
	FilterGammaContrastBrightness::Parameter para;

	para.gamma      =  static_cast<double>(gammaSlider     ->value())/100.;
	para.contrast   =  static_cast<double>(contrastSlider  ->value())/100.;
	para.brightness =  static_cast<double>(brightnessSlider->value())/100.;

	imageFilter->setParameter(para);
}
