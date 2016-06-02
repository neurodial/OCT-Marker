#include "sloimagewidget.h"

#include <manager/markermanager.h>
#include <data_structure/cscan.h>
#include <data_structure/sloimage.h>
#include <data_structure/bscan.h>

SLOImageWidget::SLOImageWidget(MarkerManager& markerManger)
: markerManger(markerManger)
{
	connect(&markerManger, SIGNAL(newCScanLoaded()), this, SLOT(reladSLOImage()));
	connect(&markerManger, SIGNAL(bscanChanged(int)), this, SLOT(bscanChanged(int)));
}

SLOImageWidget::~SLOImageWidget()
{
}

void SLOImageWidget::paintEvent(QPaintEvent* event)
{
	CVImageWidget::paintEvent(event);

	QPainter painter(this);
/*
	QBrush brush(QColor(255,0,0,80));

	// painter.setBrush(brush);
	painter.fillRect(25, 0, 20, height(), brush);
*/

	QPen normalBscan;
	QPen activBscan;

	normalBscan.setWidth(2);
	normalBscan.setColor(QColor(0,0,0));


	activBscan.setWidth(2);
	activBscan.setColor(QColor(255,0,0));

	int activBScan = markerManger.getActBScan();
	const CScan& cscan = markerManger.getCScan();
	const std::vector<BScan*> bscans = cscan.getBscans();

	const ScaleFactor factor = cscan.getSloImage()->getScaleFactor() * (1/getImageScaleFactor());

	int bscanCounter = 0;
	for(const BScan* bscan : bscans)
	{
		if(bscanCounter == activBScan)
			painter.setPen(activBscan);
		else
			painter.setPen(normalBscan);

		const CoordSLOpx& start_px = bscan->getStart() * factor;
		const CoordSLOpx&   end_px = bscan->getEnd()   * factor;

		painter.drawLine(start_px.getX(), start_px.getY(), end_px.getX(), end_px.getY());
		// qDebug("painter.drawLine(%d, %d, %d, %d)", start_px.getX(), start_px.getY(), end_px.getX(), end_px.getY());

		++bscanCounter;
	}

	painter.end();



}

void SLOImageWidget::reladSLOImage()
{
	const CScan& cscan = markerManger.getCScan();
	const SLOImage* sloImage = cscan.getSloImage();
	if(sloImage)
		showImage(sloImage->getImage());
}

void SLOImageWidget::bscanChanged(int bscan)
{

	// qDebug("void SLOImageWidget::bscanChanged(%d)", bscan);

	repaint();
}
