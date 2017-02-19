#include "sloobjectmarker.h"

#include "sloobjectsptree.h"

#include <QGraphicsScene>

#include <markerobjects/rectitem.h>

SloObjectMarker::SloObjectMarker(OctMarkerManager* markerManager)
: SloMarkerBase(markerManager)
, graphicsScene(new QGraphicsScene(this))
{
	name = tr("Slo objects");
	id   = "SloObjects";
	icon = QIcon(":/icons/tag_blue_edit.png");


	RectItem* onhMarker = new RectItem();
	onhMarker->setDescription("ONH");
	graphicsScene->addItem(onhMarker);
	rectItems["ONH"] = onhMarker;


	QPen pen1;
	pen1.setWidth(1);
	pen1.setCosmetic(true);
	onhMarker->setPen(pen1);

	resetScene();

}

SloObjectMarker::~SloObjectMarker()
{
	delete graphicsScene;
}

void SloObjectMarker::resetScene()
{
	const double scaleFactor = getScaleFactor();

	RectItem* onhMarker = rectItems["ONH"];
	onhMarker->setRect(QRectF(0.25*scaleFactor, 0.25*scaleFactor, 0.50*scaleFactor, 0.50*scaleFactor));
}

RectItem* SloObjectMarker::getRectItem(const std::string& id)
{
	RectItems::iterator it = rectItems.find(id);
	if(it == rectItems.end())
		return nullptr;
	return it->second;
}

const RectItem* SloObjectMarker::getRectItem(const std::string& id) const
{
	RectItems::const_iterator it = rectItems.find(id);
	if(it == rectItems.end())
		return nullptr;
	return it->second;
}


void SloObjectMarker::loadState(boost::property_tree::ptree& markerTree)
{
	resetScene();
	SloObjectsPtree::parsePTree(markerTree, this);
}

void SloObjectMarker::saveState(boost::property_tree::ptree& markerTree)
{
	SloObjectsPtree::fillPTree(markerTree, this);

}

