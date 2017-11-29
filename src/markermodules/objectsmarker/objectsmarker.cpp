#include "objectsmarker.h"

#include <QMouseEvent>

#include <octdata/datastruct/series.h>
#include <octdata/datastruct/bscan.h>

#include <markerobjects/rectitem.h>
#include <widgets/bscanmarkerwidget.h>

#include "objectsmarkerscene.h"
#include "objectsmarkerptree.h"

#include <data_structure/scalefactor.h>

#include"widgetobjectmarker.h"


namespace
{
	QPointF toScene(const QPoint& p, const ScaleFactor& factor)
	{
		return QPointF(p.x()/factor.getFactorX(), p.y()/factor.getFactorY());
	}

	QPointF toScene(const QPoint& p, const BScanMarkerWidget& markerWidget)
	{
		return toScene(p, markerWidget.getImageScaleFactor());
	}
}



Objectsmarker::Objectsmarker(OctMarkerManager* markerManager)
: BscanMarkerBase(markerManager)
, graphicsScene(new ObjectsmarkerScene(objectsfactory, this))
, widget(new WidgetObjectMarker)
{
	name = tr("Objects marker");
	id   = "ObjectsMarker";
	icon = QIcon(":/icons/typicons_mod/object_marker.svg");

	connect(graphicsScene, &ObjectsmarkerScene::addObjectModeChanged        , widget       , &WidgetObjectMarker::addObjectStatusChangedSlot);
	connect(widget       , &WidgetObjectMarker::addObjectStatusChangedSignal, graphicsScene, &ObjectsmarkerScene::setAddObjectMode          );
}

Objectsmarker::~Objectsmarker()
{
}


bool Objectsmarker::keyPressEvent(QKeyEvent* event, BScanMarkerWidget*)
{
	switch(event->key())
	{
		case Qt::Key_Delete:
			removeItems(graphicsScene->selectedItems());
			break;

		case Qt::Key_A:
			graphicsScene->setAddObjectMode(true);
			break;

		case Qt::Key_Escape:
			graphicsScene->setAddObjectMode(false);
			break;
		default:
			return false;
	}
	return true;
}

void Objectsmarker::loadState(boost::property_tree::ptree& markerTree)
{
	removeAllItems();
	ObjectsMarkerPTree::parsePTree(markerTree, this, objectsfactory);
	if(itemsList.size() > actBScanSceneNr)
		graphicsScene->markersFromList(itemsList.at(actBScanSceneNr));
}


void Objectsmarker::saveState(boost::property_tree::ptree& markerTree)
{
	if(itemsList.size() > actBScanSceneNr)
	{
		graphicsScene->markersToList(itemsList.at(actBScanSceneNr));
		ObjectsMarkerPTree::fillPTree(markerTree, this);
		graphicsScene->markersFromList(itemsList.at(actBScanSceneNr));
	}
}


void Objectsmarker::removeItems(const QList<QGraphicsItem*>& items)
{
	for(QGraphicsItem* item : items)
	{
		graphicsScene->removeItem(item);
		delete item;
	}
}

      QGraphicsScene* Objectsmarker::getGraphicsScene()       { return graphicsScene; }
const QGraphicsScene* Objectsmarker::getGraphicsScene() const { return graphicsScene; }


void Objectsmarker::newSeriesLoaded(const OctData::Series* series, boost::property_tree::ptree& markerTree)
{
	resetMarkerObjects(series);
	loadState(markerTree);
}


void Objectsmarker::removeAllItems()
{
	std::size_t actBScan = getActBScanNr();
	if(actBScan > itemsList.size())
		graphicsScene->markersToList(itemsList.at(actBScan));
	else
		graphicsScene->clear();

	for(std::vector<RectItem*>& items : itemsList)
	{
		for(RectItem* item : items)
			delete item;
		items.clear();
	}
}


void Objectsmarker::resetMarkerObjects(const OctData::Series* series)
{
	if(!series)
		return;

	const std::size_t numBscans = series->bscanCount();

	removeAllItems();
	if(numBscans > 0)
		itemsList.resize(numBscans);
	else
		itemsList.clear();
}



void Objectsmarker::setActBScan(std::size_t bscan)
{
	if(bscan == actBScanSceneNr)
		return;

	graphicsScene->markersToList(itemsList.at(actBScanSceneNr));
	graphicsScene->markersFromList(itemsList.at(bscan));
	actBScanSceneNr = bscan;
}

QWidget* Objectsmarker::getWidget()
{
	return widget;
}


void Objectsmarker::drawMarker(QPainter& p, BScanMarkerWidget* markerWidget, const QRect& drawrect) const
{
	if(!isActivated)
	{
		const ScaleFactor& factor = markerWidget->getImageScaleFactor();
		const double factorX = factor.getFactorX();
		const double factorY = factor.getFactorY();

		QRectF sourceRect(drawrect.x()/factorX, drawrect.y()/factorY, drawrect.width()/factorX, drawrect.height()/factorY);
		graphicsScene->render(&p, drawrect, sourceRect, Qt::IgnoreAspectRatio);
	}
	else
		BscanMarkerBase::drawMarker(p, markerWidget, drawrect);
}

/*

BscanMarkerBase::RedrawRequest Objectsmarker::mousePressEvent(QMouseEvent* event, BScanMarkerWidget* markerWidget)
{
	if(!markerWidget)
		return RedrawRequest();

	QPointF p = toScene(event->pos(), *markerWidget);

	QGraphicsItem* item = graphicsScene->itemAt(p, QTransform());
	if(item)
	{
		graphicsScene->sendEvent(item, event);
	}

	RedrawRequest rr;
	rr.redraw = true;
	return rr;
}

BscanMarkerBase::RedrawRequest Objectsmarker::mouseMoveEvent(QMouseEvent* event, BScanMarkerWidget* markerWidget)
{
}

BscanMarkerBase::RedrawRequest Objectsmarker::mouseReleaseEvent(QMouseEvent* event, BScanMarkerWidget* markerWidget)
{
}
*/
