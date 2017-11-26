#include "paintmarker.h"


#include<QPainter>
#include<QPaintEvent>


#include<widgets/bscanmarkerwidget.h>

#include <manager/octmarkermanager.h>

#include <markermodules/bscanmarkerbase.h>


void PaintMarker::paintMarker(QPaintEvent* event, BScanMarkerWidget* widget) const
{

	OctMarkerManager& manager = OctMarkerManager::getInstance();
	BscanMarkerBase* actBscanMarker = manager.getActBscanMarker();

	QPainter painter(widget);

	for(const PaintMarkerItem& pmi : model.getMarkers())
	{
		if(pmi.isShowed())
		{
			const BscanMarkerBase* marker = pmi.getMarker();
			if(marker != actBscanMarker && marker)
			{
				marker->drawMarker(painter, widget, event->rect());
			}
		}

	}

	if(actBscanMarker)
		actBscanMarker->drawMarker(painter, widget, event->rect());

	painter.end();
}
