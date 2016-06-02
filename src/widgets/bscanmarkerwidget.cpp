#include "bscanmarkerwidget.h"

#include <manager/markermanager.h>
#include <data_structure/cscan.h>
#include <data_structure/bscan.h>

BScanMarkerWidget::BScanMarkerWidget(MarkerManager& markerManger)
: CVImageWidget()
, markerManger(markerManger)
{

	connect(&markerManger, SIGNAL(newCScanLoaded()), this, SLOT(cscanLoaded()));
	connect(&markerManger, SIGNAL(bscanChanged(int)), this, SLOT(bscanChanged(int)));
}


BScanMarkerWidget::~BScanMarkerWidget()
{

}


void BScanMarkerWidget::paintEvent(QPaintEvent* event)
{
	CVImageWidget::paintEvent(event);
		// Display the image
	QPainter painter(this);

	QBrush brush(QColor(255,0,0,80));

	// painter.setBrush(brush);
	painter.fillRect(25, 0, 20, height(), brush);

	painter.end();
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
