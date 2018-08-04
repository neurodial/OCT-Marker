#ifndef EDITSPLINE_H
#define EDITSPLINE_H

#include"editbase.h"

#include<vector>


#include<data_structure/point2d.h>


#include<QPoint>

class QRubberBand;


class EditSpline : public EditBase
{
public:
	struct SplinePoint : public Point2D
	{
		SplinePoint() = default;
		explicit SplinePoint(const Point2D& p) : Point2D(p) {}

		bool marked = false;
	};
	typedef std::vector<SplinePoint> PointList;
	typedef PointList::iterator  PointIterator;
	typedef PointList::const_iterator  PointConstIterator;

private:

	OctData::Segmentationlines::Segmentline* segLine = nullptr;


	PointList supportingPoints;
// 	std::vector<double> interpolated;

	void recalcInterpolation();
	mutable QRubberBand* rubberBand = nullptr;
	QPoint rubberBandOrigin;

	std::size_t startMovePosX = 0;
	bool movePoint = false;
	bool pointMoved = false;
	PointIterator  baseEditPoint;
// 	PointIterator firstEditPoint;
// 	PointIterator  lastEditPoint;
// 	PointIterator   actEditPoint;

	void paintPoints(QPainter& painter, const ScaleFactor& factor) const;

	void resetMarkedPoints(QRect& rect);
	bool testInsertPoint(const Point2D& p, const ScaleFactor& scaleFactor);
	BscanMarkerBase::RedrawRequest movePointEvent(QMouseEvent* event, BScanMarkerWidget* widget);

	bool deleteSelectedPoints();
	BscanMarkerBase::RedrawRequest deleteMarkedPoints();

	void resetEditPoints();
	void reducePoints();
	void calcSupportPoints();

	std::tuple<PointIterator, double> findNextPoint(const Point2D& clickPoint, const ScaleFactor& scaleFactor);

	double reduceFactor = 1;
public:
	EditSpline(BScanLayerSegmentation* base);
	virtual ~EditSpline() {}

	virtual void drawMarker(QPainter& painter, BScanMarkerWidget* widget, const QRect& /*drawrect*/, const ScaleFactor& scaleFactor) const override;

	virtual BscanMarkerBase::RedrawRequest mouseMoveEvent   (QMouseEvent*, BScanMarkerWidget*) override;
	virtual BscanMarkerBase::RedrawRequest mousePressEvent  (QMouseEvent*, BScanMarkerWidget*) override;
	virtual BscanMarkerBase::RedrawRequest mouseReleaseEvent(QMouseEvent*, BScanMarkerWidget*) override;
	virtual void contextMenuEvent(QContextMenuEvent* event) override;


	virtual bool keyPressEvent(QKeyEvent*, BScanMarkerWidget*) override;

	void segLineChanged(OctData::Segmentationlines::Segmentline* segLine) override;
};

#endif // EDITSPLINE_H
