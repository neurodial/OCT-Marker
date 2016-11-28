#include "sloimagewidget.h"

#include <QPainter>
#include <QResizeEvent>

#include <octdata/datastruct/series.h>
#include <octdata/datastruct/sloimage.h>
#include <octdata/datastruct/coordslo.h>
#include <octdata/datastruct/bscan.h>

#include <manager/octmarkermanager.h>
#include <manager/octdatamanager.h>

#include <markermodules/bscanmarkerbase.h>
#include <markermodules/slomarkerbase.h>

#include <data_structure/intervalmarker.h>
#include <data_structure/programoptions.h>

#include <QGraphicsView>
#include <QGraphicsTextItem>

SLOImageWidget::SLOImageWidget(OctMarkerManager& markerManger)
: markerManger(markerManger)
, drawBScans(ProgramOptions::sloShowBscans())
, drawOnylActBScan(ProgramOptions::sloShowOnylActBScan())
{
	OctDataManager& octDataManager = OctDataManager::getInstance();
	connect(&octDataManager, &OctDataManager::seriesChanged     , this, &SLOImageWidget::reladSLOImage   );
	connect(&markerManger  , &OctMarkerManager::bscanChanged    , this, &SLOImageWidget::bscanChanged    );
	connect(&markerManger  , &OctMarkerManager::sloMarkerChanged, this, &SLOImageWidget::sloMarkerChanged);

	setMinimumSize(150,150);
	setFocusPolicy(Qt::StrongFocus);

	gv = new QGraphicsView(this);
	gv->setStyleSheet("QGraphicsView { border-style: none; background: transparent;}" );
	gv->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	gv->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	gv->setWindowFlags (Qt::FramelessWindowHint);
	gv->setCacheMode(QGraphicsView::CacheBackground);
	gv->setFocusPolicy(Qt::NoFocus);
	gv->setDragMode(QGraphicsView::NoDrag);

	// gv->show();
	// gv->setGeometry(rect());
	// gv->setVisible(ProgramOptions::sloShowLabels());
	setGraphicsViewSize(size());
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
	const OctData::Series* series           = OctDataManager::getInstance().getSeries();
	if(!series)
		return;
	
	const OctData::Series::BScanList bscans = series->getBScans();

	const OctData::SloImage& sloImage = series->getSloImage();

	const OctData::ScaleFactor     factor    = sloImage.getScaleFactor() * (1./getImageScaleFactor());
	const OctData::CoordSLOpx      shift     = sloImage.getShift()       * (getImageScaleFactor());
	const OctData::CoordTransform& transform = sloImage.getTransform();
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

			if(!drawOnylActBScan)
			{
				painter.setPen(normalBscan);
				paintBScan(painter, *bscan, factor, shift, transform, bscanCounter, true);
			}
		}

		if(actBScan)
		{
			painter.setPen(activBscan);
			paintBScan(painter, *actBScan, factor, shift, transform, -1, false);
		}
	}

	painter.end();
}

void SLOImageWidget::paintBScan(QPainter& painter, const OctData::BScan& bscan, const OctData::ScaleFactor& factor, const OctData::CoordSLOpx& shift, const OctData::CoordTransform& transform, int bscanNr, bool paintMarker)
{
	if(bscan.getCenter())
	{
		const OctData::CoordSLOpx&  start_px = bscan.getStart()  * factor;
		const OctData::CoordSLOpx& center_px = bscan.getCenter() * factor;

		double radius = center_px.abs(start_px);

		painter.drawEllipse(QPointF(center_px.getXf(), center_px.getYf()), radius, radius);
	}
	else
		paintBScanLine(painter, bscan, factor, shift, transform, bscanNr, paintMarker);
}


void SLOImageWidget::paintBScanLine(QPainter& painter, const OctData::BScan& bscan, const OctData::ScaleFactor& factor, const OctData::CoordSLOpx& shift, const OctData::CoordTransform& transform, int bscanNr, bool paintMarker)
{
	const OctData::CoordSLOpx& start_px = (transform * bscan.getStart())*factor + shift;
	const OctData::CoordSLOpx&   end_px = (transform * bscan.getEnd()  )*factor + shift;

	painter.drawLine(start_px.getX(), start_px.getY(), end_px.getX(), end_px.getY());

	if(paintMarker)
	{
		BscanMarkerBase* actMarker = markerManger.getActBscanMarker();
		if(actMarker)
			actMarker->drawBScanSLOLine(painter, bscanNr, start_px, end_px, this);
	}
}


void SLOImageWidget::reladSLOImage()
{
	const OctData::Series* series = OctDataManager::getInstance().getSeries();
	if(!series)
		return;
	const OctData::SloImage& sloImage = series->getSloImage();
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

void SLOImageWidget::showOnylActBScan(bool show)
{
	drawOnylActBScan = show;
	update();
}


void SLOImageWidget::setImageSize(QSize size)
{
	CVImageWidget::setImageSize(size);
	setGraphicsViewSize(size);
}

void SLOImageWidget::setGraphicsViewSize(QSize size)
{
/*
	QPoint p = imageWidget->pos();
	QRect  r = imageWidget->rect();*/
	gv->setGeometry(0, 0, scaledImageWidth(), scaledImageHight());
//
// 	// gv->setPos(imageWidget->pos());
//	gv->resetTransform();
// //	gv->scale(getImageScaleFactor(), getImageScaleFactor());
// //	gv->scale(getImageScaleFactor()*scaledImageWidth(), getImageScaleFactor()*scaledImageHight());
// 	gv->scale(scaledImageWidth(), scaledImageHight());

	gv->setSceneRect(0, 0, 100, 100);
	gv->fitInView(QRectF(0, 0, 100, 100));
}



void SLOImageWidget::sloMarkerChanged(SloMarkerBase* marker)
{
	if(marker)
	{
		scene = marker->getGraphicsScene();
	}
	else
	{
		scene = nullptr;
	}
	gv->setScene(scene);
	gv->setVisible(scene != nullptr);
	setGraphicsViewSize(QSize());
}

