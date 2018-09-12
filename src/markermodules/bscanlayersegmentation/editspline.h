/*
 * Copyright (c) 2018 Kay Gawlik <kaydev@amarunet.de> <kay.gawlik@beuth-hochschule.de> <kay.gawlik@charite.de>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef EDITSPLINE_H
#define EDITSPLINE_H

#include"editbase.h"

#include<vector>


#include<data_structure/point2d.h>
#include<data_structure/rect2d.h>


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

		bool isInside(double minX, double minY, double maxX, double maxY)
		{
			return (getX() >= minX && getX() <= maxX
			     && getY() >= minY && getY() <= maxY);
		}
		bool isInside(const Rect2D& r) { return isInside(r.getXMin(), r.getYMin(), r.getXMax(), r.getYMax()); }
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
	QRect deleteMarkedPoints();

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
