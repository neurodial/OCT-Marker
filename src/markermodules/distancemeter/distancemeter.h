/*
 * <one line to give the program's name and a brief idea of what it does.>
 * Copyright (C) 2018  kay <email>
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

class QLabel;

class DistanceMeter : public BscanMarkerBase
{
	Q_OBJECT

public:

	DistanceMeter(OctMarkerManager* markerManager);
	~DistanceMeter();

	virtual QWidget* getWidget   ()          override               { return widgetPtr2WGScanClassifier; }

	virtual RedrawRequest mouseMoveEvent   (QMouseEvent*, BScanMarkerWidget*) override;
	virtual RedrawRequest mousePressEvent  (QMouseEvent*, BScanMarkerWidget*) override;
	virtual RedrawRequest mouseReleaseEvent(QMouseEvent*, BScanMarkerWidget*) override;

	virtual bool leaveWidgetEvent (QEvent*     , BScanMarkerWidget*) override;

	virtual void drawMarker(QPainter&, BScanMarkerWidget*, const QRect&) const override;

	virtual void newSeriesLoaded(const OctData::Series*, boost::property_tree::ptree&) override;


	virtual void setActBScan(std::size_t bscan) override;

private:
	struct Position
	{
		bool valid = false;

		double x_milliMeter = 0;
		double y_milliMeter = 0;
		double z_milliMeter = 0;

		int x_px = 0;
		int y_px = 0;
		std::size_t bscan = 0;

		double distance(const Position& other) const;
	};

	Position clickPos;

	QWidget* widgetPtr2WGScanClassifier = nullptr;

	const OctData::Series* series = nullptr;
	const OctData::BScan*  actBScan = nullptr;
	std::size_t actBscanNr = 0;

	QLabel* distanceLabel;

	Position calcPosition(QMouseEvent* event, BScanMarkerWidget* widget);
};
