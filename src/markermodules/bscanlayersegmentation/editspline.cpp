#include "editspline.h"

#include<QPainter>
#include <QMouseEvent>

#include <widgets/bscanmarkerwidget.h>

#include"douglaspeuckeralgorithm.h"
#include"pchip.h"



namespace
{
	void paintPolygon(QPainter& painter, const std::vector<Point2D>& polygon, double factor)
	{
		for(const Point2D& p : polygon)
			painter.drawEllipse(p.getX()*factor-4, p.getY()*factor-4, 8, 8);

#if true
		if(polygon.size() < 2)
			return;

		painter.setPen(QPen(Qt::green));

		      std::vector<Point2D>::const_iterator it    = polygon.begin();
		const std::vector<Point2D>::const_iterator itEnd = polygon.end();
		Point2D lastPoint = *it;
		++it;
		while(it != itEnd)
		{
			painter.drawLine(lastPoint.getX()*factor, lastPoint.getY()*factor, it->getX()*factor, it->getY()*factor);
			lastPoint = *it;
			++it;
		}
		painter.setPen(QPen(Qt::red));
#endif
	}

}

void EditSpline::drawMarker(QPainter& painter, BScanMarkerWidget* widget, const QRect&, double scaleFactor) const
{
	paintPolygon(painter, supportingPoints, scaleFactor);
	BScanMarkerWidget::paintSegmentationLine(painter, getBScanHight(), interpolated, scaleFactor);
}

BscanMarkerBase::RedrawRequest EditSpline::mouseMoveEvent(QMouseEvent* event, BScanMarkerWidget* widget)
{
	if(!actEditPoint || !event || !widget)
		return BscanMarkerBase::RedrawRequest();

	double scaleFactor = widget->getImageScaleFactor();
	actEditPoint->setX(event->x()/scaleFactor);
	actEditPoint->setY(event->y()/scaleFactor);

	recalcInterpolation();

	BscanMarkerBase::RedrawRequest request;
	request.redraw = true;
	return request;
}

BscanMarkerBase::RedrawRequest EditSpline::mousePressEvent(QMouseEvent* event, BScanMarkerWidget* widget)
{
	double scaleFactor = widget->getImageScaleFactor();

	Point2D clickPoint(event->x()/scaleFactor, event->y()/scaleFactor);

	double minDist = std::numeric_limits<double>::infinity();
	Point2D* minDistPoint = nullptr;
	for(Point2D& p : supportingPoints)
	{
		double dist = p.euklidDist(clickPoint);
		if(dist < minDist)
		{
			minDist = dist;
			minDistPoint = &p;
		}
	}

	if(minDist < 10*scaleFactor)
		actEditPoint = minDistPoint;
	else
		actEditPoint = nullptr;

	return BscanMarkerBase::RedrawRequest();
}

BscanMarkerBase::RedrawRequest EditSpline::mouseReleaseEvent(QMouseEvent*, BScanMarkerWidget*)
{
	actEditPoint = nullptr;
	return BscanMarkerBase::RedrawRequest();
}

void EditSpline::segLineChanged(OctData::Segmentationlines::Segmentline* segLine)
{
	if(this->segLine)
	{
		this->segLine->clear();
		std::copy(interpolated.begin(), interpolated.end(), std::back_inserter(*this->segLine));
	}

	this->segLine = segLine;

	if(!segLine)
		return;

	std::vector<Point2D> vals;
	for(std::size_t x = 0; x < segLine->size(); ++x)
	{
		double val = (*segLine)[x];
		if(val < 1000 && val > 0)
		{
			vals.push_back(Point2D(x, val));
		}
	}
	DouglasPeuckerAlgorithm alg(vals);

	supportingPoints.clear();
	std::copy(alg.getPoints().begin(), alg.getPoints().end(),  std::back_inserter(supportingPoints));

	recalcInterpolation();
}

void EditSpline::recalcInterpolation()
{
	PChip pchip(supportingPoints, segLine->size());

	interpolated = pchip.getValues();
}


