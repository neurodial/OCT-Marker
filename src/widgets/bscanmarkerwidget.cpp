#include "bscanmarkerwidget.h"

#include <manager/markermanager.h>
#include <data_structure/cscan.h>
#include <data_structure/bscan.h>
#include <data_structure/intervallmarker.h>

#include <QWheelEvent>
#include <QPainter>

BScanMarkerWidget::BScanMarkerWidget(MarkerManager& markerManger)
: CVImageWidget()
, markerManger(markerManger)
{

	connect(&markerManger, SIGNAL(newCScanLoaded()), this, SLOT(cscanLoaded()));
	connect(&markerManger, SIGNAL(bscanChanged(int)), this, SLOT(bscanChanged(int)));

	connect(&markerManger, SIGNAL(markerMethodChanged(Method)), this, SLOT(markersMethodChanged()));

	connect(this, SIGNAL(bscanChangeInkrement(int)), &markerManger, SLOT(inkrementBScan(int)));

	
	setFocusPolicy(Qt::ClickFocus);
	setMouseTracking(true);
}


BScanMarkerWidget::~BScanMarkerWidget()
{
}


void BScanMarkerWidget::paintEvent(QPaintEvent* event)
{
	CVImageWidget::paintEvent(event);

	if(!markerManger.cscanLoaded())
		return;

	QPainter painter(this);

	for(const MarkerManager::MarkerMap::interval_mapping_type pair : markerManger.getMarkers())
	{
		IntervallMarker::Marker marker = pair.second;
		if(marker.isDefined())
		{
			boost::icl::discrete_interval<int> itv  = pair.first;
			painter.fillRect(itv.lower(), 0, itv.upper()-itv.lower(), height(), QColor(marker.getRed(), marker.getGreen(), marker.getBlue(),  60));
		}
	}
	

	if(mouseInWidget && markerManger.getMarkerMethod() == MarkerManager::Method::Paint)
	{
		painter.drawLine(mousePos.x(), 0, mousePos.x(), height());

		if(markerActiv)
		{
			const IntervallMarker::Marker& marker = markerManger.getActMarker();
			if(mousePos.x() != clickPos.x())
			{
				painter.drawLine(clickPos.x(), 0, clickPos.x(), height());
				QPen pen;
				pen.setColor(QColor(marker.getRed(), marker.getGreen(), marker.getBlue(), 255));
				pen.setWidth(5);
				painter.setPen(pen);
				painter.drawLine(mousePos, clickPos);
			}
		}
	}

	painter.end();
}

void BScanMarkerWidget::bscanChanged(int bscanNR)
{
	const CScan& cscan = markerManger.getCScan();
	const BScan* bscan = cscan.getBScan(bscanNR);
	if(bscan)
		showImage(bscan->getImage());
}

void BScanMarkerWidget::leaveEvent(QEvent* e)
{
	QWidget::leaveEvent(e);

	mouseInWidget = false;
	update();
}


void BScanMarkerWidget::cscanLoaded()
{
	bscanChanged(markerManger.getActBScan());
}


void BScanMarkerWidget::wheelEvent(QWheelEvent* wheelE)
{
	int deltaWheel = wheelE->delta();
	if(deltaWheel < 0)
		emit(bscanChangeInkrement(-1));
	else
		emit(bscanChangeInkrement(+1));
}


void BScanMarkerWidget::mouseMoveEvent(QMouseEvent* event)
{
	QWidget::mouseMoveEvent(event);
	
	mouseInWidget = true;
	// if(markerActiv)
	{
		mousePos = event->pos();
		update();
	}
}

void BScanMarkerWidget::mousePressEvent(QMouseEvent* event)
{
	QWidget::mousePressEvent(event);

	if(event->button() == Qt::LeftButton)
	{
		clickPos = event->pos();
		mousePos = event->pos();
		markerActiv = true;
	}
	else
		markerActiv = false;
		
}

void BScanMarkerWidget::mouseReleaseEvent(QMouseEvent* event)
{
	QWidget::mouseReleaseEvent(event);

	switch(markerManger.getMarkerMethod())
	{
		case MarkerManager::Method::Paint:
			if(clickPos.x() != event->x() && markerActiv)
			{
				// std::cout << __FUNCTION__ << ": " << clickPos << " - " << event->x() << std::endl;
				markerManger.setMarker(clickPos.x(), event->x());
			}
			break;
		case MarkerManager::Method::Fill:
			if(markerActiv)
				markerManger.fillMarker(clickPos.x());
			break;
	}
	
	markerActiv = false;
	repaint();
}

void BScanMarkerWidget::keyPressEvent(QKeyEvent* e)
{
	QWidget::keyPressEvent(e);
	
	switch(e->key())
	{
		case Qt::Key_Escape:
			markerActiv = false;
			repaint();
			break;
		case Qt::Key_Left:
			emit(bscanChangeInkrement(-1));
			break;
		case Qt::Key_Right:
			emit(bscanChangeInkrement( 1));
			break;
	}
	
}

void BScanMarkerWidget::contextMenuEvent(QContextMenuEvent* event)
{
	QWidget::contextMenuEvent(event);
}

void BScanMarkerWidget::markersMethodChanged()
{
	switch(markerManger.getMarkerMethod())
	{
		case MarkerManager::Method::Paint:
			setMouseTracking(true);
			break;
		default:
			setMouseTracking(false);
			break;
	}
	update();
}
