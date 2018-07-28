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

#include "distancemeter.h"

#define _USE_MATH_DEFINES
#include<cmath>

#include<QMouseEvent>
#include<QLabel>

#include<octdata/datastruct/series.h>

#include<data_structure/scalefactor.h>
#include<widgets/bscanmarkerwidget.h>
#include <octdata/datastruct/bscan.h>

DistanceMeter::DistanceMeter(OctMarkerManager* markerManager)
: BscanMarkerBase(markerManager)
, distanceLabel(new QLabel)
{
	name = tr("distance meter");
	id   = "DistanceMeter";
	icon = QIcon(":/icons/own/ruler.svgz");

	widgetPtr2WGScanClassifier = distanceLabel;
}

DistanceMeter::~DistanceMeter()
{
}

void DistanceMeter::drawMarker(QPainter&, BScanMarkerWidget*, const QRect&) const
{
}

bool DistanceMeter::leaveWidgetEvent(QEvent*, BScanMarkerWidget*)
{
}

DistanceMeter::Position DistanceMeter::calcPosition(QMouseEvent* event, BScanMarkerWidget* widget)
{
	if(!actBScan)
		return Position();
// 	int bScanHeight = getBScanHight();
	const ScaleFactor& scaleFactor = widget->getImageScaleFactor();

	double xPx = event->x()/scaleFactor.getFactorX();
	double yPx = event->y()/scaleFactor.getFactorY();


	int width  = actBScan->getWidth();
	int height = actBScan->getHeight();

	if(width == 0 || height == 0)
		return Position();


	OctData::CoordSLOmm sloPos = actBScan->getFracPos(xPx/width);


	Position p;
	p.x_milliMeter = sloPos.getX();
	p.y_milliMeter = sloPos.getY();
	p.z_milliMeter = yPx*actBScan->getScaleFactor().getZ();

	p.valid = true;
	p.x_px = xPx;
	p.y_px = yPx;
	p.bscan = actBscanNr;

	return p;
}


BscanMarkerBase::RedrawRequest DistanceMeter::mouseMoveEvent(QMouseEvent* event, BScanMarkerWidget* widget)
{
	if(clickPos.valid)
	{
		Position actPos = calcPosition(event, widget);

		double distance = clickPos.distance(actPos);
		distanceLabel->setText(QString("%1").arg(distance));
// 		qDebug("%lf", clickPos.distance(actPos));
	}
}

BscanMarkerBase::RedrawRequest DistanceMeter::mousePressEvent(QMouseEvent* event, BScanMarkerWidget* widget)
{
	clickPos = calcPosition(event, widget);

	qDebug("%lf  %lf  %lf", clickPos.x_milliMeter, clickPos.y_milliMeter, clickPos.z_milliMeter);

}

BscanMarkerBase::RedrawRequest DistanceMeter::mouseReleaseEvent(QMouseEvent* event, BScanMarkerWidget* widget)
{
}

void DistanceMeter::newSeriesLoaded(const OctData::Series* s, boost::property_tree::ptree& ptree)
{
	BscanMarkerBase::newSeriesLoaded(s, ptree);
	series = s;
	actBScan = nullptr;
}

void DistanceMeter::setActBScan(std::size_t bscan)
{
	if(series)
	{
		actBScan = series->getBScan(bscan);
	}
}


double DistanceMeter::Position::distance(const DistanceMeter::Position& other) const
{
	const double x = x_milliMeter - other.x_milliMeter;
	const double y = y_milliMeter - other.y_milliMeter;
	const double z = z_milliMeter - other.z_milliMeter;

	return std::sqrt(x*x + y*y + z*z);
}
