#include "editpen.h"


#include <QPainter>
#include <QMouseEvent>

#include <widgets/bscanmarkerwidget.h>


void EditPen::drawMarker(QPainter& painter, BScanMarkerWidget* widget, const QRect&, const ScaleFactor& scaleFactor) const
{
}

void EditPen::updateMinMaxX(const EditPen::SegPoint& p)
{
	const std::size_t x = p.x;
	if(actPaintMaxX < x)
		actPaintMaxX = x;
	else if(actPaintMinX > x)
		actPaintMinX = x;
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
	updateMinMaxX(segPoint);
	return request;
}

BscanMarkerBase::RedrawRequest EditPen::mousePressEvent(QMouseEvent* event, BScanMarkerWidget* widget)
{
	if(event->button() == Qt::LeftButton)
	{
		paintSegLine = true;
		lineChanged  = false;
		const ScaleFactor& scaleFactor = widget->getImageScaleFactor();
		lastPoint = calcPoint(event->x(), event->y(), scaleFactor, getBScanWidth());
		actPaintMaxX = lastPoint.x;
		actPaintMinX = lastPoint.x;
	}
	return BscanMarkerBase::RedrawRequest();
}

BscanMarkerBase::RedrawRequest EditPen::mouseReleaseEvent(QMouseEvent* event, BScanMarkerWidget* widget)
{
	if(!paintSegLine || !segLine)
		return BscanMarkerBase::RedrawRequest();

	BscanMarkerBase::RedrawRequest redraw = smoothMinMaxIntervall(widget->getImageScaleFactor());

	if(lineChanged)
		rangeModified(actPaintMinX, actPaintMaxX+1);
	lineChanged  = false;
	paintSegLine = false;
	return redraw;
}


namespace
{
	class Filter
	{
		double value  = 0;
		double weight = 0;
	public:
		double getValue()                                        const { if(weight > 0) return value/weight; return std::numeric_limits<double>::quiet_NaN(); }
		void addValue(double v, double w)
		{
			if(std::isnan(v) || v > 1e8)
				return;
			value += v*w;
			weight += w;
		}
	};
}

BscanMarkerBase::RedrawRequest EditPen::smoothMinMaxIntervall(const ScaleFactor& scaleFactor)
{
	const std::size_t minPos = std::max(static_cast<std::size_t>(2), actPaintMinX  );
	const std::size_t endPos = std::min(segLine->size()-3          , actPaintMaxX+1);

	if(minPos > endPos)
		return BscanMarkerBase::RedrawRequest();

	std::vector<double> temp(endPos - minPos);
	std::vector<double>::iterator it = temp.begin();

	for(std::size_t x = minPos; x < endPos; ++x)
	{
		Filter f;

		f.addValue((*segLine)[x-2], 1.);
		f.addValue((*segLine)[x-1], 2.);
		f.addValue((*segLine)[x  ], 3.);
		f.addValue((*segLine)[x+1], 2.);
		f.addValue((*segLine)[x+2], 1.);
		*it = f.getValue();
		++it;
	}

	std::copy(temp.begin(), temp.end(), segLine->begin() + minPos);

	BscanMarkerBase::RedrawRequest request;
	request.redraw = true;
	request.rect = getWidgetPaintSize(minPos, endPos, scaleFactor);
	return request;
}




void EditPen::segLineChanged(OctData::Segmentationlines::Segmentline* segLine)
{
	paintSegLine = false;
	this->segLine = segLine;
}


void EditPen::setLinePoint2Point(const EditPen::SegPoint& p1, const EditPen::SegPoint& p2, OctData::Segmentationlines::Segmentline& segLine)
{
	lineChanged = true;
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
}


QRect EditPen::getWidgetPaintSize(const EditPen::SegPoint& p1, const EditPen::SegPoint& p2, const ScaleFactor& scaleFactor)
{
	return getWidgetPaintSize(p1.x, p2.x, scaleFactor);
}

QRect EditPen::getWidgetPaintSize(std::size_t x1, std::size_t x2, const ScaleFactor& scaleFactor)
{
	const int broder = 2;

	int minX = static_cast<int>((static_cast<double>(std::min(x1, x2) - broder))*scaleFactor.getFactorX());
	int maxX = static_cast<int>((static_cast<double>(std::max(x1, x2) + broder))*scaleFactor.getFactorY());

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
