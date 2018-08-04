#include "editspline.h"

#include<algorithm>

#include<QPainter>
#include<QMouseEvent>
#include<QRubberBand>

#include <widgets/bscanmarkerwidget.h>

#include <data_structure/programoptions.h>
#include <data_structure/scalefactor.h>

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

		static void addPoints(QRect& rec, EditSpline::PointConstIterator begin, const EditSpline::PointConstIterator end)
		{
			while(begin != end)
			{
				addPoint(rec, *begin);
				++begin;
			}
		}

		static void addPoints2Rec(QRect& rec, EditSpline::PointConstIterator p, const EditSpline::PointList& vec, const int addPos = 0)
		{
			addPoint(rec, *p);
			if(addPos > 0)
			{
				for(int pos = 0; pos < addPos; ++pos)
				{
					if(p+1 == vec.end())
						break;
					++p;
					addPointPrivat(rec, *p);
				}
			}
			else if(addPos < 0)
			{
				for(int pos = 0; pos > addPos; --pos)
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

	QRect createRec(const Point2D& p)
	{
		double x = p.getX();
		double y = p.getY();

		return QRect(static_cast<int>(x)
		           , static_cast<int>(y)
		           , static_cast<int>(1)
		           , static_cast<int>(1));
	}


	QRect& operator*=(QRect& rect, const ScaleFactor& factor)
	{
		int x     (static_cast<int>(rect.x     ()*factor.getFactorX()));
		int y     (static_cast<int>(rect.y     ()*factor.getFactorY()));
		int width (static_cast<int>(rect.width ()*factor.getFactorX()));
		int height(static_cast<int>(rect.height()*factor.getFactorY()));
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

	void updateRec4Paint(QRect& rect, const ScaleFactor& scaleFactor)
	{
		rect *= scaleFactor;
		int adjustSize = ProgramOptions::layerSegSplinePointSize() + 4;
		rect.adjust(-adjustSize, -adjustSize, adjustSize, adjustSize);
	}

	void paintPoint(QPainter& painter, const Point2D& p, const ScaleFactor& factor, int size)
	{
		painter.drawEllipse(static_cast<int>(p.getX()*factor.getFactorX()-size/2)
		                  , static_cast<int>(p.getY()*factor.getFactorY()-size/2)
		                  , size
		                  , size);
	}

	FindSupportingPoints::Config getFindSupportingPointsConfig()
	{
		FindSupportingPoints::Config conf;
		conf.insertTol   = ProgramOptions::layerSegFindPointInsertTol  ();
		conf.maxAbsError = ProgramOptions::layerSegFindPointMaxAbsError();
		conf.removeTol   = ProgramOptions::layerSegFindPointRemoveTol  ();
		conf.maxPoints   = ProgramOptions::layerSegFindPointMaxPoints  ();

		return conf;
	}
}


EditSpline::EditSpline(BScanLayerSegmentation* base)
: EditBase(base)
,  baseEditPoint(supportingPoints.end())
// , firstEditPoint(supportingPoints.end())
// ,  lastEditPoint(supportingPoints.end())
{

}



void EditSpline::paintPoints(QPainter& painter, const ScaleFactor& factor) const
{
	painter.setPen(QPen(ProgramOptions::layerSegActiveLineColor()));
	painter.setBrush(QBrush(Qt::black));
	bool paintActivePoint = false;

	const int pointSize = ProgramOptions::layerSegSplinePointSize();

	for(const SplinePoint& p : supportingPoints)
	{
		if(paintActivePoint != p.marked)
		{
			if(p.marked) painter.setBrush(QBrush(Qt::green));
			else         painter.setBrush(QBrush(Qt::black));
			paintActivePoint = p.marked;
		}
		paintPoint(painter, p, factor, pointSize);
	}
}



void EditSpline::drawMarker(QPainter& painter, BScanMarkerWidget* widget, const QRect&, const ScaleFactor& scaleFactor) const
{
	paintPoints(painter, scaleFactor);
}

BscanMarkerBase::RedrawRequest EditSpline::movePointEvent(QMouseEvent* event, BScanMarkerWidget* widget)
{
	if(baseEditPoint == supportingPoints.end())
		return BscanMarkerBase::RedrawRequest();

	if(!movePoint || !event || !widget)
		return BscanMarkerBase::RedrawRequest();


	Point2D oldPoint = *baseEditPoint;

	const double minPos = (baseEditPoint == supportingPoints.begin())?              0:(baseEditPoint-1)->getX()+1;
	const double maxPos = (baseEditPoint == supportingPoints.end()-1)?getBScanWidth():(baseEditPoint+1)->getX()-1;

	const ScaleFactor& scaleFactor = widget->getImageScaleFactor();
	double newXVal = std::min(maxPos, std::max(minPos, std::round(event->x()/scaleFactor.getFactorX())));
	double newYVal = std::min(static_cast<double>(getBScanHight()), std::max(0., event->y()/scaleFactor.getFactorY()));

	baseEditPoint->setX(newXVal);
	baseEditPoint->setY(newYVal);
	recalcInterpolation();

	QRect repaintRect = createRec(oldPoint, *baseEditPoint);
	RecPointAdder::addPoints2Rec(repaintRect, baseEditPoint, supportingPoints, pointDrawPos);
	RecPointAdder::addPoints2Rec(repaintRect, baseEditPoint, supportingPoints, pointDrawNeg);

	updateRec4Paint(repaintRect, scaleFactor);
	BscanMarkerBase::RedrawRequest request;
	request.redraw = true;
	request.rect   = repaintRect;

	pointMoved = true;

	return request;
}


BscanMarkerBase::RedrawRequest EditSpline::mouseMoveEvent(QMouseEvent* event, BScanMarkerWidget* widget)
{
	if(movePoint)
		return movePointEvent(event, widget);


	BscanMarkerBase::RedrawRequest request;

	if(rubberBand && rubberBand->isVisible())
	{
		QRect rAlt = rubberBand->geometry();
		rubberBand->setGeometry(QRect(rubberBandOrigin, event->pos()).normalized());
		QRect rNeu = rubberBand->geometry();

		int pointSize = ProgramOptions::layerSegSplinePointSize();

		const ScaleFactor& scaleFactor = widget->getImageScaleFactor();
		double minY = rNeu.y     ()/scaleFactor.getFactorY();
		double minX = rNeu.x     ()/scaleFactor.getFactorX();
		double maxY = rNeu.height()/scaleFactor.getFactorY() + minY;
		double maxX = rNeu.width ()/scaleFactor.getFactorX() + minX;

// 		qDebug("%lf %lf %lf %lf", minX, minY, maxX, maxY);

		for(SplinePoint& point : supportingPoints)
		{
			point.marked = (point.getX() >= minX && point.getX() <= maxX
			             && point.getY() >= minY && point.getY() <= maxY);
		}
		request.redraw = true;
		request.rect   = rNeu.united(rAlt);
		request.rect.adjust(-pointSize, -pointSize, +pointSize, +pointSize);
	}

	return request;
}

bool EditSpline::testInsertPoint(const Point2D& insertPoint, const ScaleFactor& scaleFactor)
{
	if(!segLine)
		return false;

	int pX = static_cast<int>(std::round(insertPoint.getX()));
	if(pX >= 0 && pX < segLine->size())
	{
		double pY = insertPoint.getY();
		if(std::abs(segLine->at(pX) - pY) < 10./scaleFactor.getFactorY())
		{
			for(PointIterator p = supportingPoints.begin(); p != supportingPoints.end(); ++p)
			{
				if(p->getX() > pX)
				{
					baseEditPoint = supportingPoints.insert(p, SplinePoint(insertPoint));
					recalcInterpolation();
					return true;
				}
			}

		}
	}
	return false;
}


std::tuple<EditSpline::PointIterator, double> EditSpline::findNextPoint(const Point2D& clickPoint, const ScaleFactor& scaleFactor)
{
	double minDist = std::numeric_limits<double>::infinity();
	PointIterator minDistPoint = supportingPoints.end();
	for(PointIterator p = supportingPoints.begin(); p != supportingPoints.end(); ++p)
	{
		double dist = p->euklidDist(clickPoint, scaleFactor);
		if(dist < minDist)
		{
			minDist = dist;
			minDistPoint = p;
		}
	}
	return std::make_pair(minDistPoint, minDist);
}

void EditSpline::resetMarkedPoints(QRect& rect)
{
	for(SplinePoint& p : supportingPoints)
	{
		if(p.marked)
		{
			RecPointAdder::addPoint(rect, p);
			p.marked = false;
		}
	}
}


BscanMarkerBase::RedrawRequest EditSpline::mousePressEvent(QMouseEvent* event, BScanMarkerWidget* widget)
{
	BscanMarkerBase::RedrawRequest redraw;
	QRect repaintRect;
// 	if(firstEditPoint != supportingPoints.end())
// 		RecPointAdder::addPoints(repaintRect, firstEditPoint, lastEditPoint);
// 	if(lastEditPoint  != supportingPoints.end())
// 		RecPointAdder::addPoint(repaintRect, *lastEditPoint);

	bool modPress = (event->modifiers() & Qt::KeyboardModifier::ShiftModifier);

	if(!modPress)
		resetMarkedPoints(repaintRect);


	const ScaleFactor& scaleFactor = widget->getImageScaleFactor();
	Point2D clickPoint(event->x()/scaleFactor.getFactorX(), event->y()/scaleFactor.getFactorY());

	double minDist = 0;
	PointIterator minDistPoint;
	std::tie(minDistPoint, minDist) = findNextPoint(clickPoint, scaleFactor);
	bool clickOnPoint = minDist < ProgramOptions::layerSegSplinePointSize()+2;

	if(event->button() == Qt::RightButton)
	{
		if(clickOnPoint && !modPress)
		{
			minDistPoint->marked = true;
			redraw = deleteMarkedPoints();
			repaintRect = redraw.rect;
		}
	}
	else
	{
		if(modPress)
		{
			if(clickOnPoint)
			{
				bool marked = false;
				for(PointIterator p = supportingPoints.begin(); p != supportingPoints.end(); ++p)
				{
					if(p == minDistPoint || p == baseEditPoint)
					{
						p->marked = true;
						marked = !marked;
					}
					else
						p->marked = marked;
				}
				redraw.redraw = true;
			}
		}
		else
		{
			if(clickOnPoint)
			{
				movePoint = true;
				baseEditPoint = minDistPoint;
				baseEditPoint->marked = true;
				RecPointAdder::addPoint(repaintRect, *baseEditPoint);
			}
			else
			{
				movePoint = testInsertPoint(clickPoint, scaleFactor);

				if(movePoint)
				{
					RecPointAdder::addPoints2Rec(repaintRect, baseEditPoint, supportingPoints, pointDrawPos);
					RecPointAdder::addPoints2Rec(repaintRect, baseEditPoint, supportingPoints, pointDrawNeg);
				}
				else
				{
					rubberBandOrigin = event->pos();

					if(!rubberBand)
						rubberBand = new QRubberBand(QRubberBand::Rectangle, widget);
					rubberBand->setGeometry(QRect(rubberBandOrigin, QSize()));
					rubberBand->show();
				}
			}

			startMovePosX = baseEditPoint->getX();
			redraw.redraw = true; // TODO
		}
	}


	if(redraw.redraw)
	{
		if(repaintRect.isValid())
		{
			updateRec4Paint(repaintRect, scaleFactor);
			redraw.rect   = repaintRect;
		}
	}

	event->accept();

	return redraw;
}

BscanMarkerBase::RedrawRequest EditSpline::mouseReleaseEvent(QMouseEvent*, BScanMarkerWidget*)
{
	if(rubberBand)
		rubberBand->hide();

	if(movePoint && pointMoved)
	{
		QRect repaintRect; // = createRec(*lastEditPoint, *lastEditPoint);
		RecPointAdder::addPoints2Rec(repaintRect, baseEditPoint, supportingPoints, pointDrawPos);
		RecPointAdder::addPoints2Rec(repaintRect, baseEditPoint, supportingPoints, pointDrawNeg);
		RecPointAdder::addPoint(repaintRect, Point2D(startMovePosX, 0));
		rangeModified(repaintRect.left(), repaintRect.right()+1);
	}
	movePoint  = false;
	pointMoved = false;
	return BscanMarkerBase::RedrawRequest();
}

void EditSpline::segLineChanged(OctData::Segmentationlines::Segmentline* segLine)
{
	this->segLine = segLine;

	if(!segLine)
		return;

	reduceFactor = 1;

	calcSupportPoints();
}

void EditSpline::calcSupportPoints()
{
	FindSupportingPoints alg(*segLine);

	FindSupportingPoints::Config conf = getFindSupportingPointsConfig();
	conf.removeTol   *= reduceFactor;
	conf.maxAbsError *= reduceFactor;
	alg.setConfig(conf);
	alg.calculateSupportingPoints();

	std::vector<Point2D> newPoints = alg.getSupportingPoints();
	supportingPoints.resize(newPoints.size());

	std::transform(newPoints.begin(), newPoints.end(), supportingPoints.begin(), [](const Point2D& p){ return SplinePoint(p); } );
	resetEditPoints();

	recalcInterpolation();
}


void EditSpline::contextMenuEvent(QContextMenuEvent* event)
{
	event->accept(); // disable contextmenu
}

void EditSpline::resetEditPoints()
{
// 	firstEditPoint = supportingPoints.end();
// 	 lastEditPoint = supportingPoints.end();
	 baseEditPoint = supportingPoints.end();
}


void EditSpline::recalcInterpolation() // TODO: lokale neuberechnung
{
	if(!segLine)
		return;

	std::vector<Point2D> points(supportingPoints.size());
	std::copy(supportingPoints.begin(), supportingPoints.end(), points.begin());

	PChip pchip(points, segLine->size());
	const std::vector<double>& pchipPoints = pchip.getValues();

	if(pchipPoints.size() > 0)
		*segLine = pchipPoints;

}

bool EditSpline::deleteSelectedPoints()
{
	BscanMarkerBase::RedrawRequest request = deleteMarkedPoints();
	if(request.redraw)
		requestFullUpdate();
	return request.redraw;
}



BscanMarkerBase::RedrawRequest EditSpline::deleteMarkedPoints()
{
	BscanMarkerBase::RedrawRequest redraw;
	PointIterator beginRemove = supportingPoints.end();
	std::size_t startRemoveIndex = 0;

	for(size_t index = 0; index < supportingPoints.size(); ++index)
	{
		if(supportingPoints[index].marked)
		{
			if(beginRemove == supportingPoints.end())
			{
				RecPointAdder::addPoint(redraw.rect, supportingPoints[index]);
				beginRemove = supportingPoints.begin() + index;
				startRemoveIndex = index;
			}
		}
		else
		{
			if(beginRemove != supportingPoints.end())
			{
				supportingPoints.erase(beginRemove, supportingPoints.begin() + index);
				beginRemove = supportingPoints.end();
				index = startRemoveIndex;
			}
		}
	}

	if(beginRemove != supportingPoints.end())
		supportingPoints.erase(beginRemove, supportingPoints.end());

	if(redraw.rect.isValid())
	{
		RecPointAdder::addPoints2Rec(redraw.rect, baseEditPoint, supportingPoints, pointDrawPos);
		RecPointAdder::addPoints2Rec(redraw.rect, baseEditPoint, supportingPoints, pointDrawNeg);

		recalcInterpolation();
		resetEditPoints();
		redraw.redraw = true;
		rangeModified(redraw.rect.left(), redraw.rect.right()+1);
	}

// 	if(begin != supportingPoints.end() && last != supportingPoints.end())
// 	{
// 		redraw.rect = createRec(*begin);
// 		RecPointAdder::addPoints2Rec(redraw.rect, begin, supportingPoints, pointDrawNeg);
// 		RecPointAdder::addPoints2Rec(redraw.rect, last , supportingPoints, pointDrawPos);
//
// 		++last;
// 		supportingPoints.erase(begin, last);
// 		recalcInterpolation();
// 		resetEditPoints();
//
// 		redraw.redraw = true;
//
//
// 		rangeModified(redraw.rect.left(), redraw.rect.right()+1);
// 	}
	return redraw;
}



bool EditSpline::keyPressEvent(QKeyEvent* event, BScanMarkerWidget*)
{
	switch(event->key())
	{
		case Qt::Key_Delete:
			return deleteSelectedPoints();
		case Qt::Key_R:
			reducePoints();
			return true;
	}

	return false;
}

void EditSpline::reducePoints()
{
	reduceFactor += 1.;
	calcSupportPoints();
}

