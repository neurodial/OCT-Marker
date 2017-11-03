#include "objectsmarker.h"


#include <QGraphicsScene>
#include <QMouseEvent>

#include <markerobjects/rectitem.h>

#include <widgets/bscanmarkerwidget.h>


namespace
{
	QPointF toScene(const QPoint& p, double factor)
	{
		return QPointF(p.x()/factor, p.y()/factor);
	}

	QPointF toScene(const QPoint& p, const BScanMarkerWidget& markerWidget)
	{
		return toScene(p, markerWidget.getImageScaleFactor());
	}
}



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

	double scaleFactor = 100;

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

/*
void Objectsmarker::drawMarker(QPainter& p, BScanMarkerWidget* markerWidget, const QRect& drawrect) const
{
	const double factor = markerWidget->getImageScaleFactor();
	QRect sourceRect(drawrect.x()/factor, drawrect.y()/factor, drawrect.width()/factor, drawrect.height()/factor);
	graphicsScene->render(&p, drawrect, sourceRect);
}


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
