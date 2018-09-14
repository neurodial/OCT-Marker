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


#include "distancemeter.h"

#include"wgdistancemeter.h"

#define _USE_MATH_DEFINES
#include<cmath>

#include<QMouseEvent>
#include<QLabel>
#include<QPainter>

#include<octdata/datastruct/series.h>
#include<octdata/datastruct/bscan.h>

#include<data_structure/scalefactor.h>
#include<widgets/bscanmarkerwidget.h>

DistanceMeter::DistanceMeter(OctMarkerManager* markerManager)
: BscanMarkerBase(markerManager)
, wgDistanceMeter(new WGDistanceMeter(this))
{
	name = tr("distance meter");
	id   = "DistanceMeter";
	icon = QIcon(":/icons/own/ruler.svgz");

	widgetPtr2WGDistanceMeter = wgDistanceMeter;
}

DistanceMeter::~DistanceMeter()
{
	delete wgDistanceMeter;
}

void DistanceMeter::drawMarker(QPainter& painter, BScanMarkerWidget* widget, const QRect& /*rect*/) const
{
	if(!widget)
		return;

	const ScaleFactor& sf = widget->getImageScaleFactor();

	drawPosition(painter, clickPos, sf, Qt::yellow);
	drawPosition(painter, startPos, sf, Qt::red   );

}

void DistanceMeter::drawPosition(QPainter& painter, const DistanceMeter::Position& pos, const ScaleFactor& sf, QColor c) const
{
	if(pos.valid && pos.bscanNr == actBscanNr)
	{
		int posX = static_cast<int>(pos.x_px*sf.getFactorX());
		int posY = static_cast<int>(pos.y_px*sf.getFactorY());

		QPen p;
		p.setColor(c);
		p.setWidth(3);

		painter.setPen(p);

		painter.drawLine(posX - 5, posY - 5, posX + 5, posY + 5);
		painter.drawLine(posX - 5, posY + 5, posX + 5, posY - 5);
	}
}


bool DistanceMeter::leaveWidgetEvent(QEvent*, BScanMarkerWidget*)
{
	emit(updatedActualPos(Position()));
	emit(updateMousePosDistance(Distance()));
	return false;
}

QPointF DistanceMeter::calcWidgetPosition(QMouseEvent* event, BScanMarkerWidget* widget)
{
	const ScaleFactor& scaleFactor = widget->getImageScaleFactor();

	double xPx = event->x()/scaleFactor.getFactorX();
	double yPx = event->y()/scaleFactor.getFactorY();

	return QPointF(xPx, yPx);
}


DistanceMeter::Position DistanceMeter::calcPosition(QMouseEvent* event, BScanMarkerWidget* widget)
{
	return calcPosition(calcWidgetPosition(event, widget));
}

DistanceMeter::Position DistanceMeter::calcPosition(const QPointF& p)
{
	if(!actBScan)
		return Position();

	int width  = actBScan->getWidth();
	int height = actBScan->getHeight();

	if(width == 0 || height == 0)
		return Position();


	OctData::CoordSLOmm sloPos = actBScan->getFracPos(p.x()/width);

	Position pos;
	pos.x_milliMeter = sloPos.getX();
	pos.y_milliMeter = sloPos.getY();
	pos.z_milliMeter = p.y()*actBScan->getScaleFactor().getZ();

	pos.valid = true;
	pos.x_px = static_cast<int>(p.x());
	pos.y_px = static_cast<int>(p.y());
	pos.bscanNr = actBscanNr;
	pos.bscan = actBScan;

	return pos;
}

DistanceMeter::Distance DistanceMeter::calcDistance(const DistanceMeter::Position& p)
{
	Distance d;
	if(p.valid)
	{
		d.millimeter      = startPos.distance(p);
		d.bscanMillimeter = startPos.bscanDistance(p);
		d.bscanPx         = startPos.pxDistance(p);
	}
	return d;
}



BscanMarkerBase::RedrawRequest DistanceMeter::mouseMoveEvent(QMouseEvent* event, BScanMarkerWidget* widget)
{
	if(startPos.valid)
	{
		lastWidgetPos = calcWidgetPosition(event, widget);
		Position p = calcPosition(lastWidgetPos);
		Distance d = calcDistance(p);

		updatedActualPos(p);
		updateMousePosDistance(d);
	}

	return BscanMarkerBase::RedrawRequest();
}

BscanMarkerBase::RedrawRequest DistanceMeter::mousePressEvent(QMouseEvent* event, BScanMarkerWidget* widget)
{
	if(setStartPos)
	{
		startPos = calcPosition(event, widget);
		emit(updatedStartPos(startPos));
		setNewStartPosition(false);
	}
	else
		clickPos = calcPosition(event, widget);

	Distance d = calcDistance(clickPos);
	emit(updatedClickPos(clickPos));
	emit(updateClickDistance(d));

	BscanMarkerBase::RedrawRequest r;
	r.redraw = true;

	return r;
}

void DistanceMeter::newSeriesLoaded(const OctData::Series* s, boost::property_tree::ptree& ptree)
{
	BscanMarkerBase::newSeriesLoaded(s, ptree);
	series = s;
	actBScan = nullptr;
	resetPositions();
}

void DistanceMeter::setActBScan(std::size_t bscan)
{
	if(series)
	{
		actBScan = series->getBScan(bscan);
		actBscanNr = bscan;

		if(startPos.valid)
		{
			Position p = calcPosition(lastWidgetPos);
			Distance d = calcDistance(p);

			updatedActualPos(p);
			updateMousePosDistance(d);
		}
	}
}

void DistanceMeter::resetPositions()
{
	startPos = Position();
	clickPos = Position();

	setNewStartPosition(true);

	updatedStartPos(Position());
	updatedClickPos(Position());
	updatedActualPos(Position());
	updateMousePosDistance(Distance());
	updateClickDistance(Distance());

}



double DistanceMeter::Position::distance(const DistanceMeter::Position& other) const
{
	const double x = x_milliMeter - other.x_milliMeter;
	const double y = y_milliMeter - other.y_milliMeter;
	const double z = z_milliMeter - other.z_milliMeter;

	return std::sqrt(x*x + y*y + z*z);
}

double DistanceMeter::Position::pxDistance(const DistanceMeter::Position& other) const
{
	if(bscanNr == other.bscanNr)
	{
		const double x = x_px - other.x_px;
		const double y = y_px - other.y_px;
		return std::sqrt(x*x + y*y);
	}
	return std::numeric_limits<double>::quiet_NaN();
}

double DistanceMeter::Position::bscanDistance(const DistanceMeter::Position& other) const
{
	if(bscan && bscan == other.bscan)
	{
		const OctData::ScaleFactor& sf = bscan->getScaleFactor();
		const double x = (x_px - other.x_px)*sf.getX();
		const double y = (y_px - other.y_px)*sf.getZ();
		return std::sqrt(x*x + y*y);
	}
	return std::numeric_limits<double>::quiet_NaN();
}
