#include "bscanlayersegmentation.h"

#include <widgets/bscanmarkerwidget.h>
#include <QPainter>
#include <QMouseEvent>

#include <octdata/datastruct/series.h>
#include <octdata/datastruct/bscan.h>
#include <data_structure/programoptions.h>

#include "wglayerseg.h"


#include"editbase.h"
#include"editpen.h"
#include"editspline.h"

BScanLayerSegmentation::BScanLayerSegmentation(OctMarkerManager* markerManager)
: BscanMarkerBase(markerManager)
, editMethodSpline(new EditSpline(this))
, editMethodPen(new EditPen(this))
{
	name = tr("Layer Segmentation");
	id   = "layerSegmentation";
	icon = QIcon(":/icons/seglinelayer_edit.png");

	widgetPtr2WGLayerSeg = new WGLayerSeg(this);

	actEditMethod = editMethodPen;
}

BScanLayerSegmentation::~BScanLayerSegmentation()
{
	delete editMethodSpline;
	delete editMethodPen;
}


namespace
{
// 	void paintSegmentationLine(QPainter& segPainter, int bScanHeight, const OctData::Segmentationlines::Segmentline& segLine, double factor)
// 	{
// 		double lastEnt = std::numeric_limits<OctData::Segmentationlines::SegmentlineDataType>::quiet_NaN();
// 		int xCoord = 0;
// 		for(OctData::Segmentationlines::SegmentlineDataType value : segLine)
// 		{
// 			// std::cout << value << '\n';
// 			if(!std::isnan(lastEnt) && lastEnt < bScanHeight && lastEnt > 0 && value < bScanHeight && value > 0)
// 			{
// 				segPainter.drawLine(QLineF((xCoord-1)*factor, lastEnt*factor, xCoord*factor, value*factor));
// 			}
// 			lastEnt = value;
// 			++xCoord;
// 		}
// 	}


	void paintPolygon(QPainter& painter, const std::vector<Point2D>& polygon, double factor)
	{
		if(polygon.size() < 2)
			return;

		      std::vector<Point2D>::const_iterator it    = polygon.begin();
		const std::vector<Point2D>::const_iterator itEnd = polygon.end();
		Point2D lastPoint = *it;
		++it;
		while(it != itEnd)
		{
			painter.drawEllipse(lastPoint.getX()*factor-4, lastPoint.getY()*factor-4, 8, 8);
// 			painter.drawLine(lastPoint.getX()*factor, lastPoint.getY()*factor, it->getX()*factor, it->getY()*factor);
			lastPoint = *it;
			++it;
		}
		painter.drawEllipse(lastPoint.getX()*factor, lastPoint.getY()*factor, 5, 5);
	}

}




void BScanLayerSegmentation::drawMarker(QPainter& painter, BScanMarkerWidget* widget, const QRect& rec) const
{
	int bScanHeight = getBScanHight();
	double scaleFactor = widget->getImageScaleFactor();


	QPen penEdit;
	penEdit.setColor(Qt::red);
	penEdit.setWidth(2);

	QPen penNormal;
	penNormal.setColor(Qt::blue);
	penNormal.setWidth(1);


	for(OctData::Segmentationlines::SegmentlineType type : OctData::Segmentationlines::getSegmentlineTypes())
	{
		if(type == actEditType)
			painter.setPen(penEdit);
		else
			painter.setPen(penNormal);

		BScanMarkerWidget::paintSegmentationLine(painter, bScanHeight, lines[getActBScanNr()].getSegmentLine(type), scaleFactor);
	}

// 	paintSegmentationLine(painter, bScanHeight, lines[getActBScanNr()].getSegmentLine(OctData::Segmentationlines::SegmentlineType::BM  ), scaleFactor);

// 	paintPolygon(painter, polygon, scaleFactor);
// 	paintSegmentationLine(painter, bScanHeight, interpolated, scaleFactor);
//

	if(actEditMethod)
		actEditMethod->drawMarker(painter, widget, rec, scaleFactor);
}

// BScanLayerSegmentation::SegPoint BScanLayerSegmentation::calcPoint(int x, int y, double scaleFactor, int bscanWidth)
// {
// 	if(x<0)
// 		x = 0;
//
// 	std::size_t xTransformed = static_cast<std::size_t>(x/scaleFactor);
// 	double      yTransformed =                          y/scaleFactor ;
//
// 	if(xTransformed > static_cast<std::size_t>(bscanWidth-1))
// 		xTransformed = static_cast<std::size_t>(bscanWidth-1);
//
// 	return SegPoint(xTransformed, yTransformed);
// }



BscanMarkerBase::RedrawRequest BScanLayerSegmentation::mouseMoveEvent(QMouseEvent* event, BScanMarkerWidget* widget)
{
// 	if(!paintSegLine)
// 		return BscanMarkerBase::RedrawRequest();
//
// 	double scaleFactor = widget->getImageScaleFactor();
// 	SegPoint segPoint = calcPoint(event->x(), event->y(), scaleFactor, getBScanWidth());
//
// 	OctData::Segmentationlines::Segmentline& segLine = lines[getActBScanNr()].getSegmentLine(actEditType);
//
// 	setLinePoint2Point(lastPoint, segPoint, segLine);
//
// 	BscanMarkerBase::RedrawRequest request;
// 	request.redraw = true;
// 	request.rect = getWidgetPaintSize(lastPoint, segPoint, scaleFactor);
//
// 	lastPoint = segPoint;
	if(actEditMethod)
		return actEditMethod->mouseMoveEvent(event, widget);
	return BscanMarkerBase::RedrawRequest();
}

BscanMarkerBase::RedrawRequest BScanLayerSegmentation::mousePressEvent(QMouseEvent* event, BScanMarkerWidget* widget)
{
// 	paintSegLine = true;
// 	double scaleFactor = widget->getImageScaleFactor();
// 	lastPoint = calcPoint(event->x(), event->y(), scaleFactor, getBScanWidth());
	if(actEditMethod)
		return actEditMethod->mousePressEvent(event, widget);
	return BscanMarkerBase::RedrawRequest();
}

BscanMarkerBase::RedrawRequest BScanLayerSegmentation::mouseReleaseEvent(QMouseEvent* event, BScanMarkerWidget* widget)
{
	if(actEditMethod)
		return actEditMethod->mouseReleaseEvent(event, widget);
// 	paintSegLine = false;
	return BscanMarkerBase::RedrawRequest();
}

void BScanLayerSegmentation::newSeriesLoaded(const OctData::Series* series, boost::property_tree::ptree& ptree)
{
	resetMarkers(series);

}


void BScanLayerSegmentation::resetMarkers(const OctData::Series* series)
{
	if(!series)
		return;

	const std::size_t numBscans = series->bscanCount();

	lines.clear();
	lines.resize(numBscans);

	for(std::size_t i = 0; i<numBscans; ++i)
	{
		const int bscanWidth = series->getBScan(i)->getWidth();

		for(OctData::Segmentationlines::SegmentlineType type : OctData::Segmentationlines::getSegmentlineTypes())
			lines[i].getSegmentLine(type).resize(bscanWidth);
	}

}


void BScanLayerSegmentation::setActEditLinetype(OctData::Segmentationlines::SegmentlineType type)
{
	if(type == actEditType)
		return;

	actEditType = type;


	if(actEditMethod)
		actEditMethod->segLineChanged(&lines[getActBScanNr()].getSegmentLine(actEditType));

	requestFullUpdate();
}


bool BScanLayerSegmentation::keyPressEvent(QKeyEvent* event, BScanMarkerWidget*)
{
	int key = event->key();
	switch(key)
	{
		case Qt::Key_C:
			copySegLinesFromOctData();
			return true;

// 		case Qt::Key_T:
// 			splineTest();
// 			return true;

		case Qt::Key_1:
			actEditMethod = editMethodPen;
			return true;

		case Qt::Key_2:
			actEditMethod = editMethodSpline;
			return true;
	}

	return false;
}

void BScanLayerSegmentation::copySegLinesFromOctData()
{
	const OctData::BScan* bscan = getActBScan();
	if(!bscan)
		return;

	const std::size_t actBScanNr = getActBScanNr();

	lines[actBScanNr] = bscan->getSegmentLines();

	const int bscanWidth = bscan->getWidth();
	for(OctData::Segmentationlines::SegmentlineType type : OctData::Segmentationlines::getSegmentlineTypes())
		lines[actBScanNr].getSegmentLine(type).resize(bscanWidth);

	requestFullUpdate();
}

// void BScanLayerSegmentation::splineTest()
// {
// 	OctData::Segmentationlines::Segmentline& segLine = lines[getActBScanNr()].getSegmentLine(actEditType);
//
// 	std::vector<Point2D> vals;
// 	for(std::size_t x = 0; x < segLine.size(); ++x)
// 	{
// 		double val = segLine[x];
// 		if(val < 1000 && val > 0)
// 		{
// 			vals.push_back(Point2D(x, val));
// 		}
// 	}
// 	DouglasPeuckerAlgorithm alg(vals);
//
// 	polygon.clear();
// 	std::copy(alg.getPoints().begin(), alg.getPoints().end(),  std::back_inserter(polygon));
//
// 	PChip pchip(polygon, segLine.size());
//
// 	interpolated = pchip.getValues();
// /*
// 	for(double v : interpolated)
// 		std::cout << v << std::endl;*/
//
// 	/*
// 	std::vector<double> xVal;
// 	std::vector<double> yVal;
//
// 	for(std::size_t x = 0; x < segLine.size(); ++x)
// 	{
// 		double val = segLine[x];
// 		if(val < 1000 && val > 0)
// 		{
// 			xVal.push_back(x);
// 			yVal.push_back(val);
// 		}
// 	}
//
//
// 	BSpline<double> spline(xVal.data(), static_cast<int>(xVal.size()), yVal.data(), 2.);
//
// 	std::cout << spline.getNodes().size() << std::endl;
// 	std::cout << spline.getX().size() << std::endl;
// 	*/
// }

