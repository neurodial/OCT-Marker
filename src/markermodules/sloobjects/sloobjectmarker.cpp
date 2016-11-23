#include "sloobjectmarker.h"

#include <QGraphicsScene>

SloObjectMarker::SloObjectMarker(OctMarkerManager* markerManager)
: SloMarkerBase(markerManager)
, graphicsScene(new QGraphicsScene(this))
{
	name = tr("Slo objects");
	id   = "SloObjects";
	icon = QIcon(":/icons/intervall_edit.png");

}

void SloObjectMarker::loadState(boost::property_tree::ptree& markerTree)
{
}

void SloObjectMarker::saveState(boost::property_tree::ptree& markerTree)
{
}
