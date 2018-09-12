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

#include "bscanchooserspinbox.h"


#include <octdata/datastruct/series.h>


#include <manager/octmarkermanager.h>
#include <manager/octdatamanager.h>

BScanChooserSpinBox::BScanChooserSpinBox(QWidget* parent)
: QSpinBox(parent)
{
	setAlignment(Qt::AlignRight);


	connect(&OctDataManager::getInstance(), &OctDataManager::seriesChanged, this, &BScanChooserSpinBox::configBscanChooser);

	OctMarkerManager& markerManager = OctMarkerManager::getInstance();
// 	connect(this, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), &markerManager, &OctMarkerManager::chooseBScan);
// 	connect(&markerManager, &OctMarkerManager::bscanChanged, this, &QSpinBox::setValue);

	connect(this, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, &BScanChooserSpinBox::chooserChanged);
	connect(&markerManager, &OctMarkerManager::bscanChanged, this, &BScanChooserSpinBox::bscanChanged);


	configBscanChooser();
}

void BScanChooserSpinBox::configBscanChooser()
{
	const OctData::Series* series = OctDataManager::getInstance().getSeries();

	std::size_t maxBscan = 0;
	if(series)
		maxBscan = series->bscanCount();

	setMaximum(static_cast<int>(maxBscan));
	setMinimum(maxBscan>0?1:0);
	setValue(OctMarkerManager::getInstance().getActBScanNum());

	setSuffix(QString(" / %1").arg(maxBscan));
}

void BScanChooserSpinBox::bscanChanged(int value)
{
	setValue(value +1);
}

void BScanChooserSpinBox::chooserChanged(int value)
{
	OctMarkerManager::getInstance().chooseBScan(value - 1);
}

