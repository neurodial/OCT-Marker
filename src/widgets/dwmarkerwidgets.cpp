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

#include "dwmarkerwidgets.h"

#include <manager/octmarkermanager.h>
#include <markermodules/bscanmarkerbase.h>


DWMarkerWidgets::DWMarkerWidgets(QWidget* parent)
: QDockWidget(parent)
{
	OctMarkerManager* markerManager = &OctMarkerManager::getInstance();
	setWindowTitle(tr("Marker widgets"));
	connect(markerManager, &OctMarkerManager::bscanMarkerChanged, this, &DWMarkerWidgets::markerChanged);

	markerChanged(markerManager->getActBscanMarker());
}

DWMarkerWidgets::~DWMarkerWidgets()
{
	// widgets are owned by the BscanMarker -> removed it from DockWidget scope
	QWidget* oldWidget = widget();
	if(oldWidget)
		oldWidget->setParent(nullptr);
	setWidget(nullptr);
}

void DWMarkerWidgets::markerChanged(BscanMarkerBase* marker)
{
	// widgets are owned by the BscanMarker -> removed it from DockWidget scope
	QWidget* oldWidget = widget();
	if(oldWidget)
		oldWidget->setParent(nullptr);

	if(marker)
	{
		QWidget* widget = marker->getWidget();
		setWidget(widget);
	}
	else
		setWidget(nullptr);
}

