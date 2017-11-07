#ifndef EDITSPLINE_H
#define EDITSPLINE_H

#include"editbase.h"

#include<vector>


#include<data_structure/point2d.h>


class EditSpline : public EditBase
{
	OctData::Segmentationlines::Segmentline* segLine = nullptr;

	typedef std::vector<Point2D> PointList;
	typedef PointList::iterator  PointIterator;

	PointList supportingPoints;
// 	std::vector<double> interpolated;

	void recalcInterpolation();

	bool movePoint = false;
	PointIterator  baseEditPoint;
	PointIterator firstEditPoint;
	PointIterator  lastEditPoint;
// 	PointIterator   actEditPoint;

	void paintPoints(QPainter& painter, const ScaleFactor& factor) const;

	bool testInsertPoint(const Point2D& p, const ScaleFactor& scaleFactor);

	bool deleteSelectedPoints();

	void resetEditPoints();
	void reduceMarkedPoints();

	std::tuple<PointIterator, double> findNextPoint(const Point2D& clickPoint);

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
