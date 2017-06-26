#include "editspline.h"

#include<algorithm>

#include<QPainter>
#include <QMouseEvent>

#include <widgets/bscanmarkerwidget.h>

#include"douglaspeuckeralgorithm.h"
#include"findsupportingpoints.h"
#include"pchip.h"



namespace
{
	void paintPolygon(QPainter& painter, const std::vector<Point2D>& polygon, double factor)
	{

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
		static void addPointPrivat(QRect& rec, std::vector<Point2D>::const_iterator p)
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
		static void addPoint(QRect& rec, std::vector<Point2D>::const_iterator p)
		{
			if(!rec.isValid())
			{
				rec.setX(static_cast<int>(p->getX()));
				rec.setY(static_cast<int>(p->getY()));
				rec.setWidth(1);
				rec.setHeight(1);
			}
			else
				addPointPrivat(rec, p);
		}

		static void addPoints2Rec(QRect& rec, std::vector<Point2D>::const_iterator p, const std::vector<Point2D>& vec, const ssize_t addPos = 0)
		{
			if(addPos > 0)
			{
				for(ssize_t pos = 0; pos < addPos; ++pos)
				{
					if(p+1 == vec.end())
						break;
					++p;
					addPointPrivat(rec, p);
				}
			}
			else if(addPos < 0)
			{
				for(ssize_t pos = 0; pos > addPos; --pos)
				{
					if(p == vec.begin())
						break;
					--p;
					addPointPrivat(rec, p);
				}
			}
		}
	};

	int reorderPoint(std::vector<Point2D>::iterator& p, const std::vector<Point2D>& vec)
	{
		int direction = 0;
		double x = p->getX();
		while(p != vec.begin() && (p-1)->getX() > x)
		{
			std::iter_swap(p, p-1);
			--p;
			--direction;
		}

		while((p+1) != vec.end() && (p+1)->getX() < x)
		{
			std::iter_swap(p, p+1);
			++p;
			++direction;
		}
		return direction;
	}


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

	void updateRec4Paint(QRect& rect, double scaleFactor)
	{
		rect *= scaleFactor;
		int adjustSize = 6;
		rect.adjust(-adjustSize, -adjustSize, adjustSize, adjustSize);
	}
}


void EditSpline::paintPoints(QPainter& painter, double factor) const
{
	painter.setPen(QPen(Qt::red));
	painter.setBrush(QBrush(Qt::black));
	for(const Point2D& p : supportingPoints)
		painter.drawEllipse(p.getX()*factor-4, p.getY()*factor-4, 8, 8);

	if(actEditPoint != supportingPoints.end())
	{
		painter.setBrush(QBrush(Qt::green));
		painter.drawEllipse(actEditPoint->getX()*factor-4, actEditPoint->getY()*factor-4, 8, 8);
	}
}



void EditSpline::drawMarker(QPainter& painter, BScanMarkerWidget* widget, const QRect&, double scaleFactor) const
{
	paintPolygon(painter, supportingPoints, scaleFactor);
	paintPoints(painter, scaleFactor);
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
	int pointMove = reorderPoint(actEditPoint, supportingPoints);
	recalcInterpolation();


	// calc paint rect
	int pointDrawPos =  2;
	int pointDrawNeg = -2;

	if(pointMove > 0)
		pointDrawNeg -= pointMove;
	else
		pointDrawPos -= pointMove;

	QRect repaintRect = createRec(oldPoint, *actEditPoint);
	RecPointAdder::addPoints2Rec(repaintRect, actEditPoint, supportingPoints, pointDrawPos);
	RecPointAdder::addPoints2Rec(repaintRect, actEditPoint, supportingPoints, pointDrawNeg);

	updateRec4Paint(repaintRect, scaleFactor);
	BscanMarkerBase::RedrawRequest request;
	request.redraw = true;
	request.rect   = repaintRect;

	return request;
}

bool EditSpline::testInsertPoint(const Point2D& insertPoint, double scaleFactor)
{
	int pX = static_cast<int>(std::round(insertPoint.getX()));
	if(pX >= 0 && pX < interpolated.size())
	{
		double pY = insertPoint.getY();
		if(std::abs(interpolated.at(pX) - pY) < 10./scaleFactor)
		{
			for(std::vector<Point2D>::iterator p = supportingPoints.begin(); p != supportingPoints.end(); ++p)
			{
				if(p->getX() > pX)
				{
					actEditPoint = supportingPoints.insert(p, insertPoint);
					return true;
				}
			}

		}
	}
	return false;
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


	std::vector<Point2D>::iterator lastEditPoint = actEditPoint;

	if(minDist < 10/scaleFactor)
	{
		movePoint = true;
		actEditPoint = minDistPoint;
	}
	else
	{
		actEditPoint = supportingPoints.end();
		movePoint = testInsertPoint(clickPoint, scaleFactor);
	}

	BscanMarkerBase::RedrawRequest redraw;
	if(lastEditPoint != supportingPoints.end() || actEditPoint != supportingPoints.end())
	{
		QRect repaintRect;
		if(lastEditPoint != supportingPoints.end()) RecPointAdder::addPoint(repaintRect, lastEditPoint);
		if(actEditPoint  != supportingPoints.end()) RecPointAdder::addPoint(repaintRect, actEditPoint );

		updateRec4Paint(repaintRect, scaleFactor);
		redraw.redraw = true;
		redraw.rect   = repaintRect;
	}

	return redraw;
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
// 	DouglasPeuckerAlgorithm alg(vals);
	FindSupportingPoints alg(vals);

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


bool EditSpline::keyPressEvent(QKeyEvent* event, BScanMarkerWidget*)
{
	switch(event->key())
	{
		case Qt::Key_Delete:
			if(actEditPoint != supportingPoints.end())
			{
				supportingPoints.erase(actEditPoint);
				actEditPoint = supportingPoints.end();
				recalcInterpolation();
				requestFullUpdate();
				return true;
			}
	}

	return false;
}

