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
#include<data_structure/rect2d.h>

#include <QGraphicsView>
#include <QGraphicsTextItem>

#include <cmath>
#include <limits>

namespace
{
	class GraphicsView : public QGraphicsView
	{
		double scaleFactor = 100;
	public:
		explicit GraphicsView(QWidget* parent) : QGraphicsView(parent) {}

		void setScaleFactor(double factor) { scaleFactor = factor; }
	protected:
		virtual void resizeEvent(QResizeEvent *event) override
		{
			QGraphicsView::resizeEvent(event);

			setSceneRect    (0, 0, scaleFactor, scaleFactor);
			fitInView(QRectF(0, 0, scaleFactor, scaleFactor));
		}
	};
}


SLOImageWidget::SLOImageWidget(QWidget* parent)
: CVImageWidget(parent)
, markerManger(OctMarkerManager::getInstance())
{
	OctDataManager& octDataManager = OctDataManager::getInstance();
	connect(&octDataManager, &OctDataManager::seriesChanged     , this, &SLOImageWidget::reladSLOImage   );
	connect(&markerManger  , &OctMarkerManager::bscanChanged    , this, &SLOImageWidget::bscanChanged    );
	connect(&markerManger  , &OctMarkerManager::sloViewChanged  , this, &SLOImageWidget::sloViewChanged  );
	connect(&markerManger  , &OctMarkerManager::sloMarkerChanged, this, &SLOImageWidget::sloMarkerChanged);

	connect(&ProgramOptions::sloShowsBScansPos, &OptionInt::valueChanged, this, &SLOImageWidget::setBScanVisibility);

	connect(&ProgramOptions::sloShowGrid, &OptionBool::valueChanged, this, static_cast<void (SLOImageWidget::*)(void)>(&SLOImageWidget::update));

	setBScanVisibility(ProgramOptions::sloShowsBScansPos());

	setMinimumSize(150,150);
	setFocusPolicy(Qt::StrongFocus);

	gv = new GraphicsView(this);
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
	sloMarkerChanged(markerManger.getActSloMarker());
	updateGraphicsViewSize();
}

SLOImageWidget::~SLOImageWidget()
{
	// deleteIntervallColors();
}

void SLOImageWidget::paintEvent(QPaintEvent* event)
{
	CVImageWidget::paintEvent(event);

	const OctData::Series* series = OctDataManager::getInstance().getSeries();
	if(!series)
		return;

	QPainter painter(this);
	if(drawConvexHull)
		paintConvexHull(painter, series);

	if(drawBScans)
		paintBScans(painter, series);

	if(ProgramOptions::sloShowGrid())
		paintAnalyseGrid(painter, series);

	if(markPos.show)
	{
		QPen pen(QColor(128, 0, 0));
		pen.setWidth(3);
		painter.setPen(pen);
		painter.drawLine(markPos.p.getX()-5, markPos.p.getY()-5, markPos.p.getX()+5, markPos.p.getY()+5);
		painter.drawLine(markPos.p.getX()-5, markPos.p.getY()+5, markPos.p.getX()+5, markPos.p.getY()-5);
	}

	painter.end();

}

namespace
{
	OctData::ScaleFactor operator/(const OctData::ScaleFactor& osf, const ScaleFactor& isf)
	{
		return OctData::ScaleFactor(osf.getX()/isf.getFactorX(), osf.getY()/isf.getFactorY(), 1);
	}

	OctData::CoordSLOpx operator*(const OctData::CoordSLOpx& osf, const ScaleFactor& isf)
	{
		return OctData::CoordSLOpx(osf.getXf()*isf.getFactorX(), osf.getYf()*isf.getFactorY());
	}

}


void SLOImageWidget::paintConvexHull(QPainter& painter, const OctData::Series* series)
{
	if(!series)
		return;
	const OctData::Series::BScanSLOCoordList& hull = series->getConvexHull();


	if(hull.size() < 3)
		return;

	QPen pen(QColor(128, 255, 128));
	pen.setWidth(2);
	painter.setPen(pen);

	const OctData::SloImage&       sloImage  = series->getSloImage();

	const OctData::ScaleFactor     factor    = sloImage.getScaleFactor() / getImageScaleFactor();
	const OctData::CoordSLOpx      shift     = sloImage.getShift()       * getImageScaleFactor();
	const OctData::CoordTransform& transform = sloImage.getTransform();

	for(std::size_t i = 1; i < hull.size(); ++i)
	{
		const OctData::CoordSLOpx& start_px = (transform * hull[i-1])*factor + shift;
		const OctData::CoordSLOpx&   end_px = (transform * hull[i  ])*factor + shift;

		painter.drawLine(start_px.getX(), start_px.getY(), end_px.getX(), end_px.getY());
	}
}


void SLOImageWidget::paintBScans(QPainter& painter, const OctData::Series* series)
{
	QPen normalBscanPen;
	QPen activBscanPen;

	normalBscanPen.setWidth(2);
	normalBscanPen.setColor(QColor(0,0,0));


	activBscanPen.setWidth(2);
	activBscanPen.setColor(QColor(255,0,0));


	
	const OctData::Series::BScanList bscans = series->getBScans();

	const OctData::SloImage&       sloImage  = series->getSloImage();

	const OctData::ScaleFactor     factor    = sloImage.getScaleFactor() / getImageScaleFactor();
	const OctData::CoordSLOpx      shift     = sloImage.getShift()       * getImageScaleFactor();
	const OctData::CoordTransform& transform = sloImage.getTransform();
	std::size_t activBScan                   = static_cast<std::size_t>(markerManger.getActBScanNum());
	// std::cout << cscan.getSloImage()->getShift() << " * " << (getImageScaleFactor()) << " = " << shift << std::endl;

	bool paintMarker = false;
	BscanMarkerBase* actMarker = markerManger.getActBscanMarker();
	if(actMarker)
		paintMarker = actMarker->drawingBScanOnSLO();

	if(singelBScanScan || drawOnylActBScan)
	{
		if(bscans.size() > activBScan)
		{
			const OctData::BScan* actBScan = bscans.at(static_cast<std::size_t>(activBScan));
			if(actBScan)
			{
				if(paintMarker)
					painter.setPen(normalBscanPen);
				else
					painter.setPen(activBscanPen);

				paintBScan(painter, *actBScan, factor, shift, transform, activBScan, paintMarker);
			}
		}
	}
	else
	{
		std::size_t bscanCounter = 0;
		const OctData::BScan* actBScan = nullptr;
		for(const OctData::BScan* bscan : bscans)
		{
			if(bscan)
			{
				if(bscanCounter == activBScan)
				{
					actBScan = bscan;
				}
				else
				{
					painter.setPen(normalBscanPen);
					paintBScan(painter, *bscan, factor, shift, transform, bscanCounter, paintMarker);
				}
			}
			++bscanCounter;
		}

		if(actBScan)
		{
			painter.setPen(activBscanPen);
			paintBScan(painter, *actBScan, factor, shift, transform, 0, paintMarker);
		}
	}
}

void SLOImageWidget::paintBScan(QPainter& painter, const OctData::BScan& bscan, const OctData::ScaleFactor& factor, const OctData::CoordSLOpx& shift, const OctData::CoordTransform& transform, std::size_t bscanNr, bool paintMarker)
{
	switch(bscan.getBScanType())
	{
		case OctData::BScan::BScanType::Line:
			paintBScanLine(painter, bscan, factor, shift, transform, bscanNr, paintMarker);
			break;
		case OctData::BScan::BScanType::Circle:
			paintBScanCircle(painter, bscan, factor, shift, transform, bscanNr, paintMarker);
			break;
	}
}


void SLOImageWidget::paintBScanLine(QPainter& painter, const OctData::BScan& bscan, const OctData::ScaleFactor& factor, const OctData::CoordSLOpx& shift, const OctData::CoordTransform& transform, std::size_t bscanNr, bool paintMarker)
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

void SLOImageWidget::paintBScanCircle(QPainter& painter, const OctData::BScan& bscan, const OctData::ScaleFactor& factor, const OctData::CoordSLOpx& shift, const OctData::CoordTransform& transform, std::size_t bscanNr, bool paintMarker)
{
	const OctData::CoordSLOpx&  start_px = (transform * bscan.getStart() ) * factor + shift;
	const OctData::CoordSLOpx& center_px = (transform * bscan.getCenter()) * factor + shift;

	double radius = center_px.abs(start_px);

	painter.drawEllipse(QPointF(center_px.getXf(), center_px.getYf()), radius, radius);

	if(paintMarker)
	{
		BscanMarkerBase* actMarker = markerManger.getActBscanMarker();
		if(actMarker)
			actMarker->drawBScanSLOCircle(painter, bscanNr, start_px, center_px, bscan.getClockwiseRot(), this);
	}
}

void SLOImageWidget::paintAnalyseGrid(QPainter& painter, const OctData::Series* series)
{
	if(!series)
		return;

	QPen pen;
	pen.setWidth(2);
	pen.setColor(QColor(125, 125, 255));

	painter.setPen(pen);

	const OctData::AnalyseGrid& grid = series->getAnalyseGrid();

	const std::vector<double>& diameters = grid.getDiametersMM();
	if(diameters.size() == 0)
		return;


	const OctData::SloImage&       sloImage  = series->getSloImage();
	const OctData::ScaleFactor     factor    = sloImage.getScaleFactor() / getImageScaleFactor();
	const OctData::CoordSLOpx      shift     = sloImage.getShift()       * getImageScaleFactor();
	const OctData::CoordTransform& transform = sloImage.getTransform();


	const OctData::CoordSLOpx&  centerPx = (transform * grid.getCenter()) * factor + shift;

	for(double d : diameters)
	{
		double r = d/2.;
		OctData::CoordSLOpx radius = (transform * OctData::CoordSLOmm(r, r))*factor;
		painter.drawEllipse(QPointF(centerPx.getXf(), centerPx.getYf()), radius.getXf(), radius.getYf());
	}
}

void SLOImageWidget::showPosOnBScan(double t)
{
	if(!ProgramOptions::sloShowBScanMousePos())
		return;

	const OctData::BScan* actBScan = markerManger.getActBScan();

	Rect2DInt rect(markPos.p);
	if(t<0 || t>1 || !actBScan)
	{
		markPos.show = false;
		rect.addBroder(10);
		update(rect.toQRect());
		return;
	}


	OctData::CoordSLOmm point = actBScan->getStart()*(1-t) + actBScan->getEnd()*(t); // TODO falsche Richtung?

	const OctData::Series* series            = OctDataManager::getInstance().getSeries();
	const OctData::SloImage&       sloImage  = series->getSloImage();
	const OctData::ScaleFactor     factor    = sloImage.getScaleFactor() / getImageScaleFactor();
	const OctData::CoordSLOpx      shift     = sloImage.getShift()       * getImageScaleFactor();
	const OctData::CoordTransform& transform = sloImage.getTransform();


	const OctData::CoordSLOpx& markPx = (transform * point) * factor + shift;



	markPos.show = true;
	markPos.p = Point2DInt(markPx.getX(), markPx.getY());
	rect += markPos.p;
	rect.addBroder(10);
	update(rect.toQRect());
}



void SLOImageWidget::reladSLOImage()
{
	const OctData::Series* series = OctDataManager::getInstance().getSeries();
	if(!series)
		return;
	const OctData::SloImage& sloImage = series->getSloImage();
	//if(sloImage)
		showImage(sloImage.getImage());


	singelBScanScan = (series->bscanCount() == 1);

	updateGraphicsViewSize();
// 	gv->setSceneRect(0, 0, imageWidth(), imageHight());
}

void SLOImageWidget::bscanChanged(int /*bscan*/)
{
	update();
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
	updateGraphicsViewSize();
}

void SLOImageWidget::updateGraphicsViewSize()
{
	gv->setGeometry(0, 0, scaledImageWidth(), scaledImageHeight());
}



void SLOImageWidget::sloMarkerChanged(SloMarkerBase* marker)
{
	if(marker)
	{
		scene = marker->getGraphicsScene();
		GraphicsView* gvConvert = dynamic_cast<GraphicsView*>(gv);
		if(gvConvert)
			gvConvert->setScaleFactor(marker->getScaleFactor());
	}
	else
	{
		scene = nullptr;
	}
	gv->setScene(scene);
	gv->setVisible(scene != nullptr);
	updateGraphicsViewSize();
}

void SLOImageWidget::sloViewChanged()
{
	if(singelBScanScan || drawOnylActBScan)
		update();
}


namespace
{
	double calcAbsBScanLine(const OctData::BScan& bscan, const OctData::ScaleFactor& factor, const OctData::CoordSLOpx& shift, const OctData::CoordTransform& transform, const OctData::CoordSLOpx& clickPos)
	{
		const OctData::CoordSLOpx& start_px = (transform * bscan.getStart())*factor + shift;
		const OctData::CoordSLOpx&   end_px = (transform * bscan.getEnd()  )*factor + shift;


		const OctData::CoordSLOpx geradenvektor = end_px  -start_px;
		const OctData::CoordSLOpx kraftvektor   = clickPos-start_px;
		double alpha = (kraftvektor*geradenvektor) / geradenvektor.normquadrat();
		if(alpha>0 && alpha<1)
		{
			return (geradenvektor*alpha).abs(kraftvektor);
		}
		return std::numeric_limits<double>::infinity();
	}

	double calcAbsBScanCircle(const OctData::BScan& bscan, const OctData::ScaleFactor& factor, const OctData::CoordSLOpx& shift, const OctData::CoordTransform& transform, const OctData::CoordSLOpx& clickPos)
	{
		const OctData::CoordSLOpx&  start_px = (transform * bscan.getStart() ) * factor + shift;
		const OctData::CoordSLOpx& center_px = (transform * bscan.getCenter()) * factor + shift;

		double radiusCircle = center_px.abs(start_px);
		double distCenter   = center_px.abs(clickPos);

		return std::abs(distCenter - radiusCircle);
	}


	double calcAbsBScan(const OctData::BScan& bscan, const OctData::ScaleFactor& factor, const OctData::CoordSLOpx& shift, const OctData::CoordTransform& transform, const OctData::CoordSLOpx& clickPos)
	{
		if(bscan.getCenter())
			return calcAbsBScanCircle(bscan, factor, shift, transform, clickPos);
		else
			return calcAbsBScanLine(bscan, factor, shift, transform, clickPos);
	}
}



int SLOImageWidget::getBScanNearPos(int x, int y, double tol)
{
	const OctData::Series* series            = OctDataManager::getInstance().getSeries();
	if(!series)
		return -1;
	const OctData::Series::BScanList bscans  = series->getBScans();

	const OctData::SloImage&       sloImage  = series->getSloImage();
	const OctData::ScaleFactor     factor    = sloImage.getScaleFactor() / getImageScaleFactor();
	const OctData::CoordSLOpx      shift     = sloImage.getShift()       * getImageScaleFactor();
	const OctData::CoordTransform& transform = sloImage.getTransform();

	const OctData::CoordSLOpx      clickPos(x,y);

	int    nearstScan = 0;
	double nearstDist = std::numeric_limits<double>::infinity();

	int bscanCounter = 0;
	for(const OctData::BScan* bscan : bscans)
	{
		if(bscan)
		{
			double dist = calcAbsBScan(*bscan, factor, shift, transform, clickPos);
			if(dist < nearstDist)
			{
				nearstScan = bscanCounter;
				nearstDist = dist;
			}
		}
		++bscanCounter;
	}

	if(nearstDist < tol)
		return nearstScan;
	return -1;
}

void SLOImageWidget::mousePressEvent(QMouseEvent* e)
{
	int bscan = getBScanNearPos(e->x(), e->y(), 5);
	if(bscan >= 0)
	{
		e->accept();
		OctMarkerManager::getInstance().chooseBScan(bscan);
	}
}



void SLOImageWidget::wheelEvent(QWheelEvent* wheelE)
{
	int deltaWheel = wheelE->delta();
	if(deltaWheel < 0)
		markerManger.previousBScan();
	else
		markerManger.nextBScan();
	wheelE->accept();
}


void SLOImageWidget::setBScanVisibility(int opt)
{
	switch(opt)
	{
		case 0:
			drawBScans       = false;
			drawOnylActBScan = false;
			drawConvexHull   = false;
			break;
		case 1:
			drawBScans       = true;
			drawOnylActBScan = true;
			drawConvexHull   = true;
			break;
		case 2:
			drawBScans       = true;
			drawOnylActBScan = false;
			drawConvexHull   = false;
			break;
	}
	update();
}

