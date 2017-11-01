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

	RectItem* onhMarker = new RectItem();
	// sloMarker->setSelected(true);
	graphicsScene->addItem(onhMarker);
	rectItems["ONH"] = onhMarker;


	QPen pen1;
	pen1.setWidth(1);
	pen1.setCosmetic(true);
	onhMarker->setPen(pen1);

	double scaleFactor = 1000;

// 	RectItem* onhMarker = rectItems["ONH"];
	onhMarker->setRect(QRectF(0.25*scaleFactor, 0.25*scaleFactor, 0.50*scaleFactor, 0.50*scaleFactor));
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
