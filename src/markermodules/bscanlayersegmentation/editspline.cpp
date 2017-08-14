#include "editspline.h"

#include<algorithm>

#include<QPainter>
#include <QMouseEvent>

#include <widgets/bscanmarkerwidget.h>

#include"findsupportingpoints.h"
#include"pchip.h"

#include<QTime> // TODO


namespace
{
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
		return rect;
	}

	std::ostream& operator<<(std::ostream& stream, const QRect& rect)
	{
		stream << "(" << rect.x() << ", " << rect.y() << " | " << rect.width() << ", " << rect.height() << ")";
		return stream;
	}

	void updateRec4Paint(QRect& rect, double scaleFactor)
	{
		rect *= scaleFactor;
		int adjustSize = 12;
		rect.adjust(-adjustSize, -adjustSize, adjustSize, adjustSize);
	}

	void paintPoint(QPainter& painter, const Point2D& p, double factor)
	{
		painter.drawEllipse(static_cast<int>(p.getX()*factor-4)
		                  , static_cast<int>(p.getY()*factor-4)
		                  , 8
		                  , 8);
	}
}


void EditSpline::paintPoints(QPainter& painter, double factor) const
{
	painter.setPen(QPen(Qt::red));
	painter.setBrush(QBrush(Qt::black));

	for(const Point2D& p : supportingPoints)
		paintPoint(painter, p, factor);

	if(actEditPoint != supportingPoints.end())
	{
		painter.setBrush(QBrush(Qt::green));
		paintPoint(painter, *actEditPoint, factor);
	}
}



void EditSpline::drawMarker(QPainter& painter, BScanMarkerWidget* widget, const QRect&, double scaleFactor) const
{
	paintPoints(painter, scaleFactor);
}

BscanMarkerBase::RedrawRequest EditSpline::mouseMoveEvent(QMouseEvent* event, BScanMarkerWidget* widget)
{
	if(!movePoint || !event || !widget || actEditPoint == supportingPoints.end())
		return BscanMarkerBase::RedrawRequest();
	Point2D oldPoint = *actEditPoint;

	double scaleFactor = widget->getImageScaleFactor();
	actEditPoint->setX(std::round(event->x()/scaleFactor));
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
	if(!segLine)
		return false;

	int pX = static_cast<int>(std::round(insertPoint.getX()));
	if(pX >= 0 && pX < segLine->size())
	{
		double pY = insertPoint.getY();
		if(std::abs(segLine->at(pX) - pY) < 10./scaleFactor)
		{
			for(std::vector<Point2D>::iterator p = supportingPoints.begin(); p != supportingPoints.end(); ++p)
			{
				if(p->getX() > pX)
				{
					actEditPoint = supportingPoints.insert(p, insertPoint);
					recalcInterpolation();
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
	if(event->modifiers() | Qt::Key::Key_Shift)
		firstEditPoint = actEditPoint;
	else
		firstEditPoint = supportingPoints.end();

	if(minDist < 10/scaleFactor)
	{
		movePoint = true;
		actEditPoint = minDistPoint;
	}
	else
	{
		actEditPoint = supportingPoints.end();
		movePoint = testInsertPoint(clickPoint, scaleFactor); // TODO: zeichenfenster anpassen (spline geändert!)
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

	QTime t;
	t.start();
// 	DouglasPeuckerAlgorithm alg(vals);
	FindSupportingPoints alg(vals);

	qDebug("FindSupportingPoints: %d ms", t.elapsed());

	supportingPoints.clear();
	supportingPoints = alg.getSupportingPoints();
	actEditPoint = supportingPoints.end();


	recalcInterpolation();
}

void EditSpline::recalcInterpolation() // TODO: lokale neuberechnung
{
	if(!segLine)
		return;

	PChip pchip(supportingPoints, segLine->size());
	const std::vector<double>& pchipPoints = pchip.getValues();

	if(pchipPoints.size() > 0)
		*segLine = pchipPoints;

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

