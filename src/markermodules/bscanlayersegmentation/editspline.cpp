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

	class RecPointAdder
	{
		static void addPoint(QRect& rec, std::vector<Point2D>::const_iterator p)
		{
			const int x1 = rec.x();
			const int x2 = x1 + rec.width();
			const int y1 = rec.y();
			const int y2 = y1 + rec.height();

			const int x = static_cast<int>(p->getX());
			const int y = static_cast<int>(p->getY());

			if(x < x1)
			{
				rec.setX(x);
				rec.setWidth(x2-x);
			}
			else if(x>x2)
				rec.setWidth(x-x1+1);

			if(y < y1)
			{
				rec.setY(y);
				rec.setHeight(y2-y);
			}
			else if(y>y2)
				rec.setHeight(y-y1+1);
		}

	public:
		static void addPoint2Rec(QRect& rec, std::vector<Point2D>::const_iterator p, const std::vector<Point2D>& vec, const ssize_t addPos = 0)
		{
			if(addPos > 0)
			{
				for(ssize_t pos = 0; pos < addPos; ++pos)
				{
					if(p+1 == vec.end())
						break;
					++p;
					addPoint(rec, p);
				}
			}
			else if(addPos < 0)
			{
				for(ssize_t pos = 0; pos > addPos; --pos)
				{
					if(p == vec.begin())
						break;
					--p;
					addPoint(rec, p);
				}
			}
		}
	};


	QRect createRec(const Point2D& p1, const Point2D& p2)
	{
		double x1 = std::min(p1.getX(), p2.getX());
		double x2 = std::max(p1.getX(), p2.getX());

		double y1 = std::min(p1.getY(), p2.getY());
		double y2 = std::max(p1.getY(), p2.getY());
		return QRect(static_cast<int>(x1     )
		           , static_cast<int>(y1     )
		           , static_cast<int>(x2-x1+1)
		           , static_cast<int>(y2-y1+1));
	}

	QRect& operator*=(QRect& rect, double factor)
	{
		int x     (static_cast<int>(rect.x     ()*factor));
		int y     (static_cast<int>(rect.y     ()*factor));
		int width (static_cast<int>(rect.width ()*factor));
		int height(static_cast<int>(rect.height()*factor));
		rect.setX     (x     );
		rect.setY     (y     );
		rect.setWidth (width );
		rect.setHeight(height);
	}

	std::ostream& operator<<(std::ostream& stream, const QRect& rect)
	{
		stream << "(" << rect.x() << ", " << rect.y() << " | " << rect.width() << ", " << rect.height() << ")";
		return stream;
	}

}

void EditSpline::drawMarker(QPainter& painter, BScanMarkerWidget* widget, const QRect&, double scaleFactor) const
{
	paintPolygon(painter, supportingPoints, scaleFactor);
	BScanMarkerWidget::paintSegmentationLine(painter, getBScanHight(), interpolated, scaleFactor);
}

BscanMarkerBase::RedrawRequest EditSpline::mouseMoveEvent(QMouseEvent* event, BScanMarkerWidget* widget)
{
	if(!movePoint || !event || !widget || actEditPoint == supportingPoints.end())
		return BscanMarkerBase::RedrawRequest();
	Point2D oldPoint = *actEditPoint;

	double scaleFactor = widget->getImageScaleFactor();
	actEditPoint->setX(event->x()/scaleFactor);
	actEditPoint->setY(event->y()/scaleFactor);

	QRect repaintRect = createRec(oldPoint, *actEditPoint);
	RecPointAdder::addPoint2Rec(repaintRect, actEditPoint, supportingPoints,  2);
	RecPointAdder::addPoint2Rec(repaintRect, actEditPoint, supportingPoints, -2);
	recalcInterpolation();

	repaintRect *= scaleFactor;
	int adjustSize = 6;
	repaintRect.adjust(-adjustSize, -adjustSize, adjustSize, adjustSize);
	BscanMarkerBase::RedrawRequest request;
	request.redraw = true;
	request.rect   = repaintRect;

// 	std::cout << repaintRect << std::endl;
	return request;
}

BscanMarkerBase::RedrawRequest EditSpline::mousePressEvent(QMouseEvent* event, BScanMarkerWidget* widget)
{
	double scaleFactor = widget->getImageScaleFactor();

	Point2D clickPoint(event->x()/scaleFactor, event->y()/scaleFactor);

	double minDist = std::numeric_limits<double>::infinity();
	std::vector<Point2D>::iterator minDistPoint = supportingPoints.end();
	for(std::vector<Point2D>::iterator p = supportingPoints.begin(); p != supportingPoints.end(); ++p)
	{
		double dist = p->euklidDist(clickPoint);
		if(dist < minDist)
		{
			minDist = dist;
			minDistPoint = p;
		}
	}

	if(minDist < 10*scaleFactor)
	{
		movePoint = true;
		actEditPoint = minDistPoint;
	}
	else
	{
		movePoint = false;
	}

	return BscanMarkerBase::RedrawRequest();
}

BscanMarkerBase::RedrawRequest EditSpline::mouseReleaseEvent(QMouseEvent*, BScanMarkerWidget*)
{
	movePoint = false;
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
	actEditPoint = supportingPoints.end();

	recalcInterpolation();
}

void EditSpline::recalcInterpolation()
{
	PChip pchip(supportingPoints, segLine->size());

	interpolated = pchip.getValues();

}


