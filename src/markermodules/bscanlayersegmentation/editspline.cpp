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



	class SplineRedraw
	{
		void updateRec4Paint(QRect& rect, const ScaleFactor& scaleFactor)
		{
			rect *= scaleFactor;
			int adjustSize = ProgramOptions::layerSegSplinePointSize() + 4;
			rect.adjust(-adjustSize, -adjustSize, adjustSize, adjustSize);
		}
	public:
		QRect rect;

		BscanMarkerBase::RedrawRequest getRequest(const ScaleFactor& scaleFactor)
		{
			BscanMarkerBase::RedrawRequest request;
			if(rect.isValid())
			{
				updateRec4Paint(rect, scaleFactor);

				request.redraw = true;
				request.rect   = rect;

			}
			return request;
		}

	};

}


EditSpline::EditSpline(BScanLayerSegmentation* base)
: EditBase(base)
,  baseEditPoint(supportingPoints.end())
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

	SplineRedraw request;

	Point2D oldPoint = *baseEditPoint;

	const double minPos = (baseEditPoint == supportingPoints.begin())?              0:(baseEditPoint-1)->getX()+1;
	const double maxPos = (baseEditPoint == supportingPoints.end()-1)?getBScanWidth():(baseEditPoint+1)->getX()-1;

	const ScaleFactor& scaleFactor = widget->getImageScaleFactor();
	double newXVal = std::min(maxPos, std::max(minPos, std::round(event->x()/scaleFactor.getFactorX())));
	double newYVal = std::min(static_cast<double>(getBScanHight()), std::max(0., event->y()/scaleFactor.getFactorY()));

	baseEditPoint->setX(newXVal);
	baseEditPoint->setY(newYVal);
	recalcInterpolation();

	request.rect = createRec(oldPoint, *baseEditPoint);
	RecPointAdder::addPoints2Rec(request.rect, baseEditPoint, supportingPoints, pointDrawPos);
	RecPointAdder::addPoints2Rec(request.rect, baseEditPoint, supportingPoints, pointDrawNeg);


	pointMoved = true;

	return request.getRequest(scaleFactor);
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
		QRect updateRect = rNeu.united(rAlt);

		int pointSize = ProgramOptions::layerSegSplinePointSize();

		const ScaleFactor& scaleFactor = widget->getImageScaleFactor();
		Rect2D markRect(rNeu);
		markRect.scaleXY(scaleFactor.getFactorX(), scaleFactor.getFactorY());

		Rect2D updateRect2D(updateRect);
		updateRect2D.scaleXY(scaleFactor.getFactorX(), scaleFactor.getFactorY());
// 		qDebug("%lf %lf %lf %lf", minX, minY, maxX, maxY);

		for(SplinePoint& point : supportingPoints)
		{
			if(point.isInside(updateRect2D))
				point.marked = point.isInside(markRect);
		}
		request.redraw = true;
		request.rect   = updateRect;
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
					baseEditPoint->marked = true;
					recalcInterpolation();
					pointMoved = true;
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
// 	SplineRedraw redraw;
	SplineRedraw request;
// 	QRect repaintRect;

	bool modPress = (event->modifiers() & Qt::KeyboardModifier::ShiftModifier);



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
			request.rect = deleteMarkedPoints();
		}
	}
	else
	{
		if(!modPress)
			resetMarkedPoints(request.rect);

		if(clickOnPoint)
		{
			if(modPress)
			{
				bool marked = false;
				for(PointIterator p = supportingPoints.begin(); p != supportingPoints.end(); ++p)
				{
					bool markPoint = marked;
					if(p == minDistPoint || p == baseEditPoint)
					{
						marked = !marked;
						markPoint = true;
					}
					if(p->marked != markPoint)
					{
						RecPointAdder::addPoint(request.rect, *p);
						p->marked = markPoint;
					}
				}
			}
			else
			{
				movePoint = true;
				baseEditPoint = minDistPoint;
				baseEditPoint->marked = true;
				startMovePosX = baseEditPoint->getX();

				RecPointAdder::addPoint(request.rect, *baseEditPoint);
			}
		}
		else
		{
			movePoint = testInsertPoint(clickPoint, scaleFactor);

			if(movePoint)
			{
				RecPointAdder::addPoints2Rec(request.rect, baseEditPoint, supportingPoints, pointDrawPos);
				RecPointAdder::addPoints2Rec(request.rect, baseEditPoint, supportingPoints, pointDrawNeg);
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
	}



	event->accept();

	return request.getRequest(scaleFactor);
}

BscanMarkerBase::RedrawRequest EditSpline::mouseReleaseEvent(QMouseEvent*, BScanMarkerWidget* widget)
{
	SplineRedraw redraw;
	if(rubberBand)
		rubberBand->hide();

	if(movePoint && pointMoved)
	{
		RecPointAdder::addPoints2Rec(redraw.rect, baseEditPoint, supportingPoints, pointDrawPos);
		RecPointAdder::addPoints2Rec(redraw.rect, baseEditPoint, supportingPoints, pointDrawNeg);
		RecPointAdder::addPoint(redraw.rect, Point2D(startMovePosX, 0));
		rangeModified(redraw.rect.left(), redraw.rect.right()+1);

		baseEditPoint->marked = false;
	}
	movePoint  = false;
	pointMoved = false;

	return redraw.getRequest(widget->getImageScaleFactor());
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
	QRect request = deleteMarkedPoints();
	if(request.isValid())
		requestFullUpdate();
	return request.isValid();
}



QRect EditSpline::deleteMarkedPoints()
{
	QRect redraw;
	QRect removeRect;
	PointIterator beginRemove = supportingPoints.end();
	std::size_t startRemoveIndex = 0;

	for(size_t index = 0; index < supportingPoints.size(); ++index)
	{
		if(supportingPoints[index].marked)
		{
			RecPointAdder::addPoint(removeRect, supportingPoints[index]);
			if(beginRemove == supportingPoints.end())
			{
				removeRect = QRect();
				beginRemove = supportingPoints.begin() + index;
				startRemoveIndex = index;
			}
		}
		else
		{
			if(beginRemove != supportingPoints.end())
			{
				PointIterator lastRemovePoint = supportingPoints.begin() + index;
				RecPointAdder::addPoints2Rec(removeRect, beginRemove    , supportingPoints, pointDrawNeg);
				RecPointAdder::addPoints2Rec(removeRect, lastRemovePoint, supportingPoints, pointDrawPos-1);


				supportingPoints.erase(beginRemove, lastRemovePoint);
				beginRemove = supportingPoints.end();
				index = startRemoveIndex;

				recalcInterpolation();
				rangeModified(removeRect.left(), removeRect.right()+1);

				redraw = redraw.united(removeRect);
			}
		}
	}

	if(beginRemove != supportingPoints.end()) // Delete with last point
	{
		PointIterator lastRemovePoint = supportingPoints.end();
		RecPointAdder::addPoints2Rec(removeRect, beginRemove    , supportingPoints, pointDrawNeg);
		RecPointAdder::addPoint(removeRect, *(lastRemovePoint-1));

		supportingPoints.erase(beginRemove, lastRemovePoint);

		recalcInterpolation();
		rangeModified(removeRect.left(), removeRect.right()+1);

		redraw = redraw.united(removeRect);
	}

	if(redraw.isValid())
		resetEditPoints();

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

