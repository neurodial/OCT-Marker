#include "sloimagewidget.h"

#include <QPainter>
#include <QResizeEvent>

#include <octdata/datastruct/series.h>
#include <octdata/datastruct/sloimage.h>
#include <octdata/datastruct/coordslo.h>
#include <octdata/datastruct/bscan.h>

#include <manager/markermanager.h>

#include <data_structure/intervallmarker.h>

#include <QGraphicsView>
#include <QGraphicsTextItem>

#include <markerobjects/rectitem.h>

SLOImageWidget::SLOImageWidget(MarkerManager& markerManger)
: markerManger(markerManger)
{
	connect(&markerManger, SIGNAL(newCScanLoaded()), this, SLOT(reladSLOImage()));
	connect(&markerManger, SIGNAL(bscanChanged(int)), this, SLOT(bscanChanged(int)));

	setMinimumSize(50,50);
	setFocusPolicy(Qt::StrongFocus);

	gv = new QGraphicsView(this);
	gv->setStyleSheet("background: transparent");
	gv->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	gv->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	gv->setWindowFlags (Qt::FramelessWindowHint);
	gv->setCacheMode(QGraphicsView::CacheBackground);
	gv->setFocusPolicy(Qt::NoFocus);

	scene = new QGraphicsScene(this);
	gv->setScene(scene);

	RectItem* currentRect = new RectItem();
	currentRect->setDescription("ONH");
	currentRect->setRect(QRectF(50, 50, 50, 50));
	currentRect->setSelected(true);
	scene->addItem(currentRect);


	gv->show();
	gv->setGeometry(rect());

	
	// createIntervallColors();
}

SLOImageWidget::~SLOImageWidget()
{
	// deleteIntervallColors();
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

	int activBScan                          = markerManger.getActBScan();
	const OctData::Series& series           = markerManger.getSeries();
	const OctData::Series::BScanList bscans = series.getBScans();

	const OctData::SloImage& sloImage = series.getSloImage();

	const OctData::ScaleFactor factor = sloImage.getScaleFactor() * (1./getImageScaleFactor());
	const OctData::CoordSLOpx  shift  = sloImage.getShift()       * (getImageScaleFactor());
	// std::cout << cscan.getSloImage()->getShift() << " * " << (getImageScaleFactor()) << " = " << shift << std::endl;

	if(drawBScans)
	{
		int bscanCounter = -1;
		const OctData::BScan* actBScan = nullptr;
		for(const OctData::BScan* bscan : bscans)
		{
			++bscanCounter;

			if(!bscan)
				continue;

			if(bscanCounter == activBScan)
			{
				actBScan = bscan;
				continue;
			}

			painter.setPen(normalBscan);
			paintBScan(painter, *bscan, factor, shift, bscanCounter, true);
		}

		if(actBScan)
		{
			painter.setPen(activBscan);
			paintBScan(painter, *actBScan, factor, shift, -1, false);
		}
	}

	painter.end();
}

void SLOImageWidget::paintBScan(QPainter& painter, const OctData::BScan& bscan, const OctData::ScaleFactor& factor, const OctData::CoordSLOpx& shift, int bscanNr, bool paintMarker)
{
	if(bscan.getCenter())
	{
		const OctData::CoordSLOpx&  start_px = bscan.getStart()  * factor;
		const OctData::CoordSLOpx& center_px = bscan.getCenter() * factor;

		double radius = center_px.abs(start_px);

		painter.drawEllipse(QPointF(center_px.getXf(), center_px.getYf()), radius, radius);
	}
	else
		paintBScanLine(painter, bscan, factor, shift, bscanNr, paintMarker);
}


void SLOImageWidget::paintBScanLine(QPainter& painter, const OctData::BScan& bscan, const OctData::ScaleFactor& factor, const OctData::CoordSLOpx& shift, int bscanNr, bool paintMarker)
{
	const OctData::CoordSLOpx& start_px = bscan.getStart()*factor + shift;
	const OctData::CoordSLOpx&   end_px = bscan.getEnd()  *factor + shift;

	painter.drawLine(start_px.getX(), start_px.getY(), end_px.getX(), end_px.getY());

	double bscanWidth = bscan.getWidth();
	QPen pen;
	pen.setWidth(3);

	if(paintMarker)
	{
		for(const MarkerManager::MarkerMap::interval_mapping_type pair : markerManger.getMarkers(bscanNr))
		{
			IntervallMarker::Marker marker = pair.second;
			if(marker.isDefined())
			{
				boost::icl::discrete_interval<int> itv  = pair.first;

				double f1 = static_cast<double>(itv.lower())/bscanWidth;
				double f2 = static_cast<double>(itv.upper())/bscanWidth;

				const OctData::CoordSLOpx p1 = start_px*(1.-f1) + end_px*f1;
				const OctData::CoordSLOpx p2 = start_px*(1.-f2) + end_px*f2;

				// pen.setColor(*(intervallColors.at(markerQ)));
				pen.setColor(QColor(marker.getRed(), marker.getGreen(), marker.getBlue(), 255));
				painter.setPen(pen);
				painter.drawLine(QPointF(p1.getXf(), p1.getYf()), QPointF(p2.getXf(), p2.getYf()));

				// painter.fillRect(itv.lower(), 0, itv.upper()-itv.lower(), height(), *(intervallColors.at(markerQ)));
			}
		}
	}
}


void SLOImageWidget::reladSLOImage()
{
	const OctData::Series& series = markerManger.getSeries();
	const OctData::SloImage& sloImage = series.getSloImage();
	//if(sloImage)
		showImage(sloImage.getImage());

	gv->setSceneRect(0, 0, imageWidth(), imageHight());
}

void SLOImageWidget::bscanChanged(int /*bscan*/)
{
	repaint();
}

void SLOImageWidget::showBScans(bool show)
{
	drawBScans = show;
	update();
}

void SLOImageWidget::showLabels(bool show)
{

	gv->setVisible(show);
}




void SLOImageWidget::setImageSize(QSize size)
{
	CVImageWidget::setImageSize(size);
/*
	QPoint p = imageWidget->pos();
	QRect  r = imageWidget->rect();*/
	gv->setGeometry(0, 0, scaledImageWidth(), scaledImageHight());
	// gv->setPos(imageWidget->pos());
	gv->resetTransform();
	gv->scale(getImageScaleFactor(), getImageScaleFactor());
	// gv->setSceneRect(0, 0, imageWidth(), imageHight());
}


