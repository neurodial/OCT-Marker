#include "sloimagewidget.h"

#include <manager/markermanager.h>
#include <data_structure/cscan.h>
#include <data_structure/sloimage.h>
#include <data_structure/bscan.h>
#include <data_structure/intervallmarker.h>

SLOImageWidget::SLOImageWidget(MarkerManager& markerManger)
: markerManger(markerManger)
{
	connect(&markerManger, SIGNAL(newCScanLoaded()), this, SLOT(reladSLOImage()));
	connect(&markerManger, SIGNAL(bscanChanged(int)), this, SLOT(bscanChanged(int)));

	setMinimumSize(50,50);
	
	createIntervallColors();
}

SLOImageWidget::~SLOImageWidget()
{
	deleteIntervallColors();
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

	int activBScan     = markerManger.getActBScan();
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

		if(bscan->getCenter())
		{
			const CoordSLOpx&  start_px = bscan->getStart()  * factor;
			const CoordSLOpx& center_px = bscan->getCenter() * factor;
			
			double radius = center_px.abs(start_px);

			painter.drawEllipse(QPointF(center_px.getXf(), center_px.getYf()), radius, radius);
		}
		else
		{
			const CoordSLOpx& start_px = bscan->getStart() * factor;
			const CoordSLOpx&   end_px = bscan->getEnd()   * factor;

			painter.drawLine(start_px.getX(), start_px.getY(), end_px.getX(), end_px.getY());
			
			double bscanWidth = bscan->getWidth();
			QPen pen;
			pen.setWidth(3);
			
			for(const MarkerManager::MarkerMap::interval_mapping_type pair : markerManger.getMarkers(bscanCounter))
			{
				int markerQ = pair.second;
				if(markerQ >= 0)
				{
					boost::icl::discrete_interval<int> itv  = pair.first;
					
					double f1 = static_cast<double>(itv.lower())/bscanWidth;
					double f2 = static_cast<double>(itv.upper())/bscanWidth;
					
					const CoordSLOpx p1 = start_px*(1.-f1) + end_px*f1;
					const CoordSLOpx p2 = start_px*(1.-f2) + end_px*f2;
					
					pen.setColor(*(intervallColors.at(markerQ)));
					painter.setPen(pen);
					painter.drawLine(QPointF(p1.getXf(), p1.getYf()), QPointF(p2.getXf(), p2.getYf()));
					
					// painter.fillRect(itv.lower(), 0, itv.upper()-itv.lower(), height(), *(intervallColors.at(markerQ)));
				}
			}
		
			// qDebug("painter.drawLine(%d, %d, %d, %d)", start_px.getX(), start_px.getY(), end_px.getX(), end_px.getY());
		}
			
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


void SLOImageWidget::deleteIntervallColors()
{
	for(QColor* c : intervallColors)
		delete c;

	intervallColors.clear();
}


void SLOImageWidget::createIntervallColors()
{
	deleteIntervallColors();

	const IntervallMarker& intervallMarker = IntervallMarker::getInstance();

	for(const IntervallMarker::Marker& marker : intervallMarker.getIntervallMarkerList())
		intervallColors.push_back(new QColor(marker.getRed(), marker.getGreen(), marker.getBlue(), 255));
}

