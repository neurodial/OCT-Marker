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
