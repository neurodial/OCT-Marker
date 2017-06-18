#include "bscanlayersegmentation.h"

#include <widgets/bscanmarkerwidget.h>
#include <QPainter>

#include <octdata/datastruct/series.h>
#include <octdata/datastruct/bscan.h>
#include <QMouseEvent>
#include <data_structure/programoptions.h>

#include "wglayerseg.h"


BScanLayerSegmentation::BScanLayerSegmentation(OctMarkerManager* markerManager)
: BscanMarkerBase(markerManager)
{
	name = tr("Layer Segmentation");
	id   = "layerSegmentation";
	icon = QIcon(":/icons/seglinelayer_edit.png");

	widgetPtr2WGLayerSeg = new WGLayerSeg(this);
}

namespace
{
	void paintSegmentationLine(QPainter& segPainter, int bScanHeight, const OctData::Segmentationlines::Segmentline& segLine, double factor)
	{
		double lastEnt = std::numeric_limits<OctData::Segmentationlines::SegmentlineDataType>::quiet_NaN();
		int xCoord = 0;
		for(OctData::Segmentationlines::SegmentlineDataType value : segLine)
		{
			// std::cout << value << '\n';
			if(!std::isnan(lastEnt) && lastEnt < bScanHeight && lastEnt > 0 && value < bScanHeight && value > 0)
			{
				segPainter.drawLine(QLineF((xCoord-1)*factor, lastEnt*factor, xCoord*factor, value*factor));
			}
			lastEnt = value;
			++xCoord;
		}
	}

}


QRect BScanLayerSegmentation::getWidgetPaintSize(const BScanLayerSegmentation::SegPoint& p1, const BScanLayerSegmentation::SegPoint& p2, double scaleFactor)
{
	const int broder = 2;

	int minX = static_cast<int>((static_cast<double>(std::min(p1.x, p2.x) - broder))*scaleFactor);
	int maxX = static_cast<int>((static_cast<double>(std::max(p1.x, p2.x) + broder))*scaleFactor);

// 	int minY = static_cast<int>(std::min(p1.y, p2.y)*scaleFactor) - broder;
// 	int maxY = static_cast<int>(std::max(p1.y, p2.y)*scaleFactor) + broder;

// 	QRect rect = QRect(minX, minY, maxX-minX, maxY-minY);
	QRect rect = QRect(minX, 0, maxX-minX, static_cast<int>(getBScanHight()*scaleFactor+0.5)); // old and new pos

	return rect;
}



void BScanLayerSegmentation::drawMarker(QPainter& painter, BScanMarkerWidget* widget, const QRect&) const
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

		paintSegmentationLine(painter, bScanHeight, lines[getActBScanNr()].getSegmentLine(type), scaleFactor);
	}

	paintSegmentationLine(painter, bScanHeight, lines[getActBScanNr()].getSegmentLine(OctData::Segmentationlines::SegmentlineType::BM  ), scaleFactor);
}

BScanLayerSegmentation::SegPoint BScanLayerSegmentation::calcPoint(int x, int y, double scaleFactor, int bscanWidth)
{
	if(x<0)
		x = 0;

	std::size_t xTransformed = static_cast<std::size_t>(x/scaleFactor);
	double      yTransformed =                          y/scaleFactor ;

	if(xTransformed > static_cast<std::size_t>(bscanWidth-1))
		xTransformed = static_cast<std::size_t>(bscanWidth-1);

	return SegPoint(xTransformed, yTransformed);
}

void BScanLayerSegmentation::setLinePoint2Point(const BScanLayerSegmentation::SegPoint& p1, const BScanLayerSegmentation::SegPoint& p2, OctData::Segmentationlines::Segmentline& segLine)
{
	if(p1.x == p2.x)
	{
		segLine[p2.x] = p2.y;
		return;
	}

	std::size_t pMin;
	std::size_t pMax;
	double pMinY;
	double pMaxY;

	if(p1.x < p2.x)
	{
		pMin = p1.x;
		pMax = p2.x;
		pMinY = p1.y;
		pMaxY = p2.y;
	}
	else
	{
		pMin = p2.x;
		pMax = p1.x;
		pMinY = p2.y;
		pMaxY = p1.y;
	}

	std::size_t length = pMax - pMin;
	double lengthD = static_cast<double>(length);

	for(std::size_t pos = 0; pos <= length; ++pos)
	{
		double posD = static_cast<double>(pos);
		segLine[pos+pMin] = pMinY*(1. - posD/lengthD) + pMaxY*(posD/lengthD);
	}
// 	qDebug("%lf", segLine[length+pMin]);
}


BscanMarkerBase::RedrawRequest BScanLayerSegmentation::mouseMoveEvent(QMouseEvent* event, BScanMarkerWidget* widget)
{
	if(!paintSegLine)
		return BscanMarkerBase::RedrawRequest();

	double scaleFactor = widget->getImageScaleFactor();
	SegPoint segPoint = calcPoint(event->x(), event->y(), scaleFactor, getBScanWidth());

	OctData::Segmentationlines::Segmentline& segLine = lines[getActBScanNr()].getSegmentLine(actEditType);

	setLinePoint2Point(lastPoint, segPoint, segLine);

	BscanMarkerBase::RedrawRequest request;
	request.redraw = true;
	request.rect = getWidgetPaintSize(lastPoint, segPoint, scaleFactor);

	lastPoint = segPoint;
	return request;
}

BscanMarkerBase::RedrawRequest BScanLayerSegmentation::mousePressEvent(QMouseEvent* event, BScanMarkerWidget* widget)
{
	paintSegLine = true;
	double scaleFactor = widget->getImageScaleFactor();
	lastPoint = calcPoint(event->x(), event->y(), scaleFactor, getBScanWidth());
	return BscanMarkerBase::RedrawRequest();
}

BscanMarkerBase::RedrawRequest BScanLayerSegmentation::mouseReleaseEvent(QMouseEvent*, BScanMarkerWidget*)
{
	paintSegLine = false;
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
		int bscanWidth = series->getBScan(i)->getWidth();

		for(OctData::Segmentationlines::SegmentlineType type : OctData::Segmentationlines::getSegmentlineTypes())
			lines[i].getSegmentLine(type).resize(bscanWidth);
	}

}


void BScanLayerSegmentation::setActEditLinetype(OctData::Segmentationlines::SegmentlineType type)
{
	if(type == actEditType)
		return;

	actEditType = type;

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
	}

	return false;
}

void BScanLayerSegmentation::copySegLinesFromOctData()
{
	const OctData::BScan* bscan = getActBScan();
	if(!bscan)
		return;

	lines[getActBScanNr()] = bscan->getSegmentLines();

	requestFullUpdate();
}
/*
void BScanLayerSegmentation::splineTest()
{
	std::vector<double> xVal;
	std::vector<double> yVal;

	OctData::Segmentationlines::Segmentline& segLine = lines[getActBScanNr()].getSegmentLine(actEditType);
	for(std::size_t x = 0; x < segLine.size(); ++x)
	{
		double val = segLine[x];
		if(val < 1000 && val > 0)
		{
			xVal.push_back(x);
			yVal.push_back(val);
		}
	}


	BSpline<double> spline(xVal.data(), static_cast<int>(xVal.size()), yVal.data(), 2.);

	std::cout << spline.getNodes().size() << std::endl;
	std::cout << spline.getX().size() << std::endl;
}
*/
