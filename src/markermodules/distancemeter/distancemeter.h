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


#pragma once

#include "../bscanmarkerbase.h"

#include<limits>

#include<QPointF>

class WGDistanceMeter;
class ScaleFactor;

namespace OctData { class BScan; };

class DistanceMeter : public BscanMarkerBase
{
	Q_OBJECT

public:
	struct Position
	{
		bool valid = false;

		double x_milliMeter = 0;
		double y_milliMeter = 0;
		double z_milliMeter = 0;

		int x_px = 0;
		int y_px = 0;
		std::size_t bscanNr = 0;

		const OctData::BScan* bscan = nullptr;

		double distance(const Position& other) const;
		double pxDistance(const Position& other) const;
		double bscanDistance(const Position& other) const;
	};

	struct Distance
	{
		double millimeter      = std::numeric_limits<double>::quiet_NaN();
		double bscanMillimeter = std::numeric_limits<double>::quiet_NaN();
		double bscanPx         = std::numeric_limits<double>::quiet_NaN();
	};

	DistanceMeter(OctMarkerManager* markerManager);
	~DistanceMeter();

	virtual QWidget* getWidget   ()          override               { return widgetPtr2WGDistanceMeter; }

	virtual RedrawRequest mouseMoveEvent   (QMouseEvent*, BScanMarkerWidget*) override;
	virtual RedrawRequest mousePressEvent  (QMouseEvent*, BScanMarkerWidget*) override;

	virtual bool leaveWidgetEvent (QEvent*     , BScanMarkerWidget*) override;
	virtual void drawMarker(QPainter&, BScanMarkerWidget*, const QRect&) const override;
	virtual void newSeriesLoaded(const OctData::Series*, boost::property_tree::ptree&) override;

	virtual void setActBScan(std::size_t bscan) override;

	bool isSetNewStartPosition() const                              { return setStartPos; }

	void resetPositions();

private:

	Position startPos;
	Position clickPos;
	QPointF  lastWidgetPos;

	bool setStartPos = true;

	QWidget* widgetPtr2WGDistanceMeter = nullptr;
	WGDistanceMeter* wgDistanceMeter = nullptr;

	const OctData::Series* series = nullptr;
	const OctData::BScan*  actBScan = nullptr;
	std::size_t actBscanNr = 0;

	Position calcPosition(QMouseEvent* event, BScanMarkerWidget* widget);
	QPointF  calcWidgetPosition(QMouseEvent* event, BScanMarkerWidget* widget);
	Position calcPosition(const QPointF& p);

	Distance calcDistance(const Position& p);

	void drawPosition(QPainter& painter, const Position& pos, const ScaleFactor& sf, QColor c) const;

public slots:
	virtual void setNewStartPosition(bool b)                        { if(setStartPos != b) { setStartPos = b; emit(changeSetStartPosition(b)); } }

signals:
	void updatedStartPos(Position p);
	void updatedActualPos(Position p);
	void updatedClickPos(Position p);
	void updateClickDistance(Distance d);
	void updateMousePosDistance(Distance d);

	void changeSetStartPosition(bool b);
};
