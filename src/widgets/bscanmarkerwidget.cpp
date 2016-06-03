#include "bscanmarkerwidget.h"

#include <manager/markermanager.h>
#include <data_structure/cscan.h>
#include <data_structure/bscan.h>
#include <QWheelEvent>

BScanMarkerWidget::BScanMarkerWidget(MarkerManager& markerManger)
: CVImageWidget()
, markerManger(markerManger)
{

	connect(&markerManger, SIGNAL(newCScanLoaded()), this, SLOT(cscanLoaded()));
	connect(&markerManger, SIGNAL(bscanChanged(int)), this, SLOT(bscanChanged(int)));

	connect(this, SIGNAL(bscanChangeInkrement(int)), &markerManger, SLOT(inkrementBScan(int)));
}


BScanMarkerWidget::~BScanMarkerWidget()
{

}


void BScanMarkerWidget::paintEvent(QPaintEvent* event)
{
	CVImageWidget::paintEvent(event);

	if(markerStart != markerEnd)
	{
		QPainter painter(this);

		QBrush brush(QColor(255,0,0,80));

		// painter.setBrush(brush);
		painter.fillRect(markerStart, 0, markerEnd-markerStart, height(), brush);

		painter.end();
	}
}

void BScanMarkerWidget::bscanChanged(int bscanNR)
{
	const CScan& cscan = markerManger.getCScan();
	const BScan* bscan = cscan.getBScan(bscanNR);
	if(bscan)
		showImage(bscan->getImage());
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
}

void BScanMarkerWidget::mousePressEvent(QMouseEvent* event)
{
	QWidget::mousePressEvent(event);

	clickPos = event->x();
}

void BScanMarkerWidget::mouseReleaseEvent(QMouseEvent* event)
{
	QWidget::mouseReleaseEvent(event);


	if(clickPos != event->x())
	{
		markerStart = clickPos;
		markerEnd   = event->x();
	}

	repaint();
}

