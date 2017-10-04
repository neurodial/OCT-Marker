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

	void paintPoints(QPainter& painter, double factor) const;

	bool testInsertPoint(const Point2D& p, double scaleFactor);

	bool deleteSelectedPoints();

	void resetEditPoints();

	std::tuple<PointIterator, double> findNextPoint(const Point2D& clickPoint);

public:
	EditSpline(BScanLayerSegmentation* base);
	virtual ~EditSpline() {}

	virtual void drawMarker(QPainter& painter, BScanMarkerWidget* widget, const QRect& /*drawrect*/, double scaleFactor) const override;

	virtual BscanMarkerBase::RedrawRequest mouseMoveEvent   (QMouseEvent*, BScanMarkerWidget*) override;
	virtual BscanMarkerBase::RedrawRequest mousePressEvent  (QMouseEvent*, BScanMarkerWidget*) override;
	virtual BscanMarkerBase::RedrawRequest mouseReleaseEvent(QMouseEvent*, BScanMarkerWidget*) override;


	virtual bool keyPressEvent(QKeyEvent*, BScanMarkerWidget*) override;

	void segLineChanged(OctData::Segmentationlines::Segmentline* segLine) override;
};

#endif // EDITSPLINE_H
