#include "editspline.h"

#include<algorithm>

#include<QPainter>
#include <QMouseEvent>

#include <widgets/bscanmarkerwidget.h>

#include"findsupportingpoints.h"
#include"pchip.h"

// #include<QTime> // TODO


namespace
{

	// calc paint rect
	constexpr const int pointDrawPos =  2;
	constexpr const int pointDrawNeg = -2;


	class RecPointAdder
	{
		static void addPointPrivat(QRect& rec, const Point2D& p)
		{
			const int x1 = rec.x();
			const int x2 = x1 + rec.width();
			const int y1 = rec.y();
			const int y2 = y1 + rec.height();

			const int x = static_cast<int>(p.getX());
			const int y = static_cast<int>(p.getY());

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
		static void addPoint(QRect& rec, const Point2D& p)
		{
			if(!rec.isValid())
			{
				rec.setX(static_cast<int>(p.getX()));
				rec.setY(static_cast<int>(p.getY()));
				rec.setWidth(1);
				rec.setHeight(1);
			}
			else
				addPointPrivat(rec, p);
		}

		static void addPoints(QRect& rec, std::vector<Point2D>::const_iterator begin, const std::vector<Point2D>::const_iterator end)
		{
			while(begin != end)
			{
				addPoint(rec, *begin);
				++begin;
			}
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
					addPointPrivat(rec, *p);
				}
			}
			else if(addPos < 0)
			{
				for(ssize_t pos = 0; pos > addPos; --pos)
				{
					if(p == vec.begin())
						break;
					--p;
					addPointPrivat(rec, *p);
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


EditSpline::EditSpline(BScanLayerSegmentation* base)
: EditBase(base)
,  baseEditPoint(supportingPoints.end())
, firstEditPoint(supportingPoints.end())
,  lastEditPoint(supportingPoints.end())
{

}



void EditSpline::paintPoints(QPainter& painter, double factor) const
{
	painter.setPen(QPen(Qt::red));
	painter.setBrush(QBrush(Qt::black));

	for(const Point2D& p : supportingPoints)
		paintPoint(painter, p, factor);

	if(firstEditPoint != supportingPoints.end() && lastEditPoint != supportingPoints.end())
	{
		painter.setBrush(QBrush(Qt::green));
		PointIterator endIt = lastEditPoint + 1;
		for(PointIterator pIt = firstEditPoint; pIt != endIt; ++pIt)
			paintPoint(painter, *pIt, factor);
	}
}



void EditSpline::drawMarker(QPainter& painter, BScanMarkerWidget* widget, const QRect&, double scaleFactor) const
{
	paintPoints(painter, scaleFactor);
}

BscanMarkerBase::RedrawRequest EditSpline::mouseMoveEvent(QMouseEvent* event, BScanMarkerWidget* widget)
{
	if(lastEditPoint != firstEditPoint)
		return BscanMarkerBase::RedrawRequest();

	if(!movePoint || !event || !widget || lastEditPoint == supportingPoints.end())
		return BscanMarkerBase::RedrawRequest();
	Point2D oldPoint = *lastEditPoint;

	const double minPos = (lastEditPoint == supportingPoints.begin())?              0:(lastEditPoint-1)->getX()+1;
	const double maxPos = (lastEditPoint == supportingPoints.end()-1)?getBScanWidth():(lastEditPoint+1)->getX()-1;

	double scaleFactor = widget->getImageScaleFactor();
	double newXVal = std::min(maxPos, std::max(minPos, std::round(event->x()/scaleFactor)));
	double newYVal = std::min(static_cast<double>(getBScanHight()), std::max(0., std::round(event->y()/scaleFactor)));

	lastEditPoint->setX(newXVal);
	lastEditPoint->setY(newYVal);
	recalcInterpolation();

	QRect repaintRect = createRec(oldPoint, *lastEditPoint);
	RecPointAdder::addPoints2Rec(repaintRect, lastEditPoint, supportingPoints, pointDrawPos);
	RecPointAdder::addPoints2Rec(repaintRect, lastEditPoint, supportingPoints, pointDrawNeg);

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
					firstEditPoint = supportingPoints.insert(p, insertPoint);
					 lastEditPoint = firstEditPoint;
					recalcInterpolation();
					return true;
				}
			}

		}
	}
	return false;
}


std::tuple<std::vector<Point2D>::iterator, double> EditSpline::findNextPoint(const Point2D& clickPoint)
{
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
	return std::make_pair(minDistPoint, minDist);
}



BscanMarkerBase::RedrawRequest EditSpline::mousePressEvent(QMouseEvent* event, BScanMarkerWidget* widget)
{
	BscanMarkerBase::RedrawRequest redraw;
	QRect repaintRect;
	if(firstEditPoint != supportingPoints.end())
		RecPointAdder::addPoints(repaintRect, firstEditPoint, lastEditPoint);
	if(lastEditPoint  != supportingPoints.end())
		RecPointAdder::addPoint(repaintRect, *lastEditPoint);

	double scaleFactor = widget->getImageScaleFactor();
	Point2D clickPoint(event->x()/scaleFactor, event->y()/scaleFactor);

	double minDist = 0;
	PointIterator minDistPoint;
	std::tie(minDistPoint, minDist) = findNextPoint(clickPoint);
	bool clickOnPoint = minDist < 10/scaleFactor;

// 	qDebug("%d", event->modifiers());

	if(event->modifiers() & Qt::KeyboardModifier::ShiftModifier)
	{
		if(clickOnPoint)
		{
			if(baseEditPoint == supportingPoints.end())
				baseEditPoint = minDistPoint;
			firstEditPoint = baseEditPoint;
			lastEditPoint  = minDistPoint ;

			if(firstEditPoint > lastEditPoint)
				std::swap(firstEditPoint, lastEditPoint);

			redraw.redraw = true;
		}
	}
	else
	{
		if(clickOnPoint)
		{
			movePoint = true;
			firstEditPoint = minDistPoint;
			lastEditPoint  = minDistPoint;
		}
		else
		{
			firstEditPoint = supportingPoints.end();
			lastEditPoint  = supportingPoints.end();
			movePoint = testInsertPoint(clickPoint, scaleFactor);

			if(movePoint)
			{
				RecPointAdder::addPoints2Rec(repaintRect, lastEditPoint, supportingPoints, pointDrawPos);
				RecPointAdder::addPoints2Rec(repaintRect, lastEditPoint, supportingPoints, pointDrawNeg);
			}
		}

		baseEditPoint = firstEditPoint;
		redraw.redraw = true; // TODO
	}


	if(redraw.redraw)
	{
		if(firstEditPoint != supportingPoints.end())
			RecPointAdder::addPoints(repaintRect, firstEditPoint, lastEditPoint);
		if(lastEditPoint  != supportingPoints.end())
			RecPointAdder::addPoint(repaintRect, *lastEditPoint);
		updateRec4Paint(repaintRect, scaleFactor);
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


// 	QTime t;
// 	t.start();
	FindSupportingPoints alg(*segLine);
	alg.calculateSupportingPoints();

// 	qDebug("FindSupportingPoints: %d ms", t.elapsed());

	supportingPoints.clear();
	supportingPoints = alg.getSupportingPoints();
	resetEditPoints();

	recalcInterpolation();
}

void EditSpline::resetEditPoints()
{
	firstEditPoint = supportingPoints.end();
	 lastEditPoint = supportingPoints.end();
	 baseEditPoint = supportingPoints.end();
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

bool EditSpline::deleteSelectedPoints()
{
	if(firstEditPoint != supportingPoints.end() && lastEditPoint != supportingPoints.end())
	{
		++lastEditPoint;
		supportingPoints.erase(firstEditPoint, lastEditPoint);
		recalcInterpolation();
		resetEditPoints();
		requestFullUpdate();
		return true;
	}
	return false;
}


bool EditSpline::keyPressEvent(QKeyEvent* event, BScanMarkerWidget*)
{
	switch(event->key())
	{
		case Qt::Key_Delete:
			return deleteSelectedPoints();
		case Qt::Key_R:
			reduceMarkedPoints();
			return true;
	}

	return false;
}

void EditSpline::reduceMarkedPoints()
{
	if(firstEditPoint != supportingPoints.end() && lastEditPoint != supportingPoints.end())
	{
// 		QTime t;
// 		t.start();

		FindSupportingPoints alg(*segLine, firstEditPoint, lastEditPoint);

		FindSupportingPoints::Config conf;
		conf.removeTol   *= 2;
		conf.maxAbsError *= 2;
		alg.setConfig(conf);

		alg.removePoints();
		PointList p = alg.getSupportingPoints();

		lastEditPoint = supportingPoints.erase(firstEditPoint+1, lastEditPoint);
		lastEditPoint = supportingPoints.insert(lastEditPoint, p.begin()+1, p.end());

		if(p.size() > 0)
			lastEditPoint += p.size() - 1;
		else
			--lastEditPoint;

// 		qDebug("FindSupportingPoints: %d ms", t.elapsed());

		recalcInterpolation();
		requestFullUpdate();
	}
}
