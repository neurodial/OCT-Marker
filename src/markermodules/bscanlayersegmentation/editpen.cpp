#include "editpen.h"


#include <QPainter>
#include <QMouseEvent>

#include <widgets/bscanmarkerwidget.h>


void EditPen::drawMarker(QPainter& painter, BScanMarkerWidget* widget, const QRect&, const ScaleFactor& scaleFactor) const
{
}

BscanMarkerBase::RedrawRequest EditPen::mouseMoveEvent(QMouseEvent* event, BScanMarkerWidget* widget)
{
	if(!paintSegLine || !segLine)
		return BscanMarkerBase::RedrawRequest();

	const ScaleFactor& scaleFactor = widget->getImageScaleFactor();
	SegPoint segPoint = calcPoint(event->x(), event->y(), scaleFactor, getBScanWidth());

// 	OctData::Segmentationlines::Segmentline& segLine = lines[getActBScanNr()].getSegmentLine(actEditType);

	setLinePoint2Point(lastPoint, segPoint, *segLine);

	BscanMarkerBase::RedrawRequest request;
	request.redraw = true;
	request.rect = getWidgetPaintSize(lastPoint, segPoint, scaleFactor);

	lastPoint = segPoint;
	return request;
}

BscanMarkerBase::RedrawRequest EditPen::mousePressEvent(QMouseEvent* event, BScanMarkerWidget* widget)
{
	paintSegLine = true;
	const ScaleFactor& scaleFactor = widget->getImageScaleFactor();
	lastPoint = calcPoint(event->x(), event->y(), scaleFactor, getBScanWidth());
	return BscanMarkerBase::RedrawRequest();
}

BscanMarkerBase::RedrawRequest EditPen::mouseReleaseEvent(QMouseEvent* event, BScanMarkerWidget* widget)
{
	paintSegLine = false;
	return BscanMarkerBase::RedrawRequest();
}



void EditPen::segLineChanged(OctData::Segmentationlines::Segmentline* segLine)
{
	this->segLine = segLine;
}


void EditPen::setLinePoint2Point(const EditPen::SegPoint& p1, const EditPen::SegPoint& p2, OctData::Segmentationlines::Segmentline& segLine)
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


QRect EditPen::getWidgetPaintSize(const EditPen::SegPoint& p1, const EditPen::SegPoint& p2, const ScaleFactor& scaleFactor)
{
	const int broder = 2;

	int minX = static_cast<int>((static_cast<double>(std::min(p1.x, p2.x) - broder))*scaleFactor.getFactorX());
	int maxX = static_cast<int>((static_cast<double>(std::max(p1.x, p2.x) + broder))*scaleFactor.getFactorY());

// 	int minY = static_cast<int>(std::min(p1.y, p2.y)*scaleFactor) - broder;
// 	int maxY = static_cast<int>(std::max(p1.y, p2.y)*scaleFactor) + broder;

// 	QRect rect = QRect(minX, minY, maxX-minX, maxY-minY);
	QRect rect = QRect(minX, 0, maxX-minX, static_cast<int>(getBScanHight()*scaleFactor.getFactorY()+0.5)); // old and new pos

	return rect;
}

EditPen::SegPoint EditPen::calcPoint(int x, int y, const ScaleFactor& scaleFactor, int bscanWidth)
{
	if(x<0)
		x = 0;

	std::size_t xTransformed = static_cast<std::size_t>(x/scaleFactor.getFactorX());
	double      yTransformed =                          y/scaleFactor.getFactorY() ;

	if(xTransformed > static_cast<std::size_t>(bscanWidth-1))
		xTransformed = static_cast<std::size_t>(bscanWidth-1);

	return SegPoint(xTransformed, yTransformed);
}
