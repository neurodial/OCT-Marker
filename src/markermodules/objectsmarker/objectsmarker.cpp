#include "objectsmarker.h"


#include <QGraphicsScene>

#include <markerobjects/rectitem.h>


Objectsmarker::Objectsmarker(OctMarkerManager* markerManager)
: BscanMarkerBase(markerManager)
, graphicsScene(new QGraphicsScene(this))
{
	name = tr("Objects marker");
	id   = "ObjectsMarker";
	icon = QIcon(":/icons/typicons_mod/object_marker.svg");

}


bool Objectsmarker::keyPressEvent(QKeyEvent*, BScanMarkerWidget*)
{
}

void Objectsmarker::loadState(boost::property_tree::ptree& markerTree)
{
}


void Objectsmarker::saveState(boost::property_tree::ptree& markerTree)
{
}
