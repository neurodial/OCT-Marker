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

#include "paintmarker.h"


#include<QPainter>
#include<QPaintEvent>


#include<widgets/bscanmarkerwidget.h>

#include <manager/octmarkermanager.h>

#include <markermodules/bscanmarkerbase.h>

PaintMarker::PaintMarker()
{
	connect(&model, &PaintMarkerModel::viewChanged, this, &PaintMarker::viewChanged);
}



void PaintMarker::paintMarker(QPaintEvent* event, BScanMarkerWidget* widget) const
{
	QPainter painter(widget);
	paintMarker(painter, widget, event->rect());
	painter.end();
}

void PaintMarker::paintMarker(QPainter& painter, BScanMarkerWidget* widget, const QRect& rect) const
{
	OctMarkerManager& manager = OctMarkerManager::getInstance();
	BscanMarkerBase* actBscanMarker = manager.getActBscanMarker();

	for(const PaintMarkerItem& pmi : model.getMarkers())
	{
		if(pmi.isShowed())
		{
			const BscanMarkerBase* marker = pmi.getMarker();
			if(marker != actBscanMarker && marker)
			{
				marker->drawMarker(painter, widget, rect);
			}
		}
	}

	if(actBscanMarker)
		actBscanMarker->drawMarker(painter, widget, rect);
}
