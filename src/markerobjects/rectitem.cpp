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

#include "rectitem.h"

#include<cmath>

#include<QPainter>
#include<QBrush>
#include<QGraphicsSceneMouseEvent>

RectItem::RectItem()
{
	setBrush(QBrush(QColor(50, 255, 50, 50)));
	setFlag(QGraphicsItem::ItemIsSelectable, true);
	setFlag(QGraphicsItem::ItemIsMovable   , true);
}



void RectItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
	QGraphicsRectItem::paint(painter, option, widget);

	if(isSelected()) // draw handles only if the item is selected
	{
		const double handlerWidth  = sizeResizeHandler*rect().width ();
		const double handlerHeight = sizeResizeHandler*rect().height();

		const QRectF r = rect();

		painter->setBrush(QBrush(QColor(50, 50, 255, 150)));
		painter->drawRect(QRectF(r.topLeft    (), r.topLeft()     + QPointF( handlerWidth,  handlerHeight)));
		painter->drawRect(QRectF(r.topRight   (), r.topRight()    + QPointF(-handlerWidth,  handlerHeight)));
		painter->drawRect(QRectF(r.bottomLeft (), r.bottomLeft()  + QPointF( handlerWidth, -handlerHeight)));
		painter->drawRect(QRectF(r.bottomRight(), r.bottomRight() + QPointF(-handlerWidth, -handlerHeight)));


		painter->setBrush(QBrush(QColor(255, 50, 50, 130)));
		double radius = std::min(rect().width(), rect().height())/5;
		painter->drawEllipse(rect().center(), radius, radius);
	}
	else if(!descripton.isEmpty())
	{
		QFont font;
		font.setPointSizeF(rect().width()/5.);
		painter->setPen(QGraphicsRectItem::pen());
		painter->setFont(font);
		painter->drawText(rect(), Qt::AlignCenter, descripton);
	}
}

namespace
{
	void flipBit(int& value, int bit)
	{
		int mask = 1 << bit;

		if(value & mask)
			value &= ~mask;
		else
			value |= mask;
	}
}

void RectItem::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
	if(resizing)
	{
		QRectF r = rect();
		QPointF dPoint = event->pos() - mousePressPos;
		switch(activeRezieHandle)
		{
			case 0: r.setTopLeft    (r.topLeft    () + dPoint); break;
			case 1: r.setTopRight   (r.topRight   () + dPoint); break;
			case 2: r.setBottomLeft (r.bottomLeft () + dPoint); break;
			case 3: r.setBottomRight(r.bottomRight() + dPoint); break;
		}
		if(r.width() > 0 && r.height() > 0)
		{
			mousePressPos = event->pos();
			setRect(r);
		}
		else
		{
			if(r.width() < 0)
				flipBit(activeRezieHandle, 0);
			if(r.height() < 0)
				flipBit(activeRezieHandle, 1);
		}
	}
	else
		QGraphicsRectItem::mouseMoveEvent(event);
}

int RectItem::insideHandler(double relPos) const
{
	if(relPos >= 0 && relPos <= sizeResizeHandler)
		return 1;
	if(relPos >= 1-sizeResizeHandler && relPos <= 1)
		return 2;
	return 0;
}


void RectItem::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
	if(isSelected())
	{
		mousePressPos = event->pos();

		const QPointF clickPos = event->pos();
		const QRectF r = rect();

		QPointF relPos = (clickPos - r.topLeft());
		double relX = relPos.x()/r.width();
		double relY = relPos.y()/r.height();

		int handlerX = insideHandler(relX);
		int handlerY = insideHandler(relY);

		if(handlerX != 0 && handlerY != 0)
		{
			activeRezieHandle = (handlerX-1) + (handlerY-1)*2;
			resizing = true;
			return;
		}
	}
	QGraphicsRectItem::mousePressEvent(event);
}

void RectItem::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
	if(resizing)
	{
		resizing = false;
		makeValid();
	}
	else
		QGraphicsRectItem::mouseReleaseEvent(event);
}

void RectItem::makeValid()
{
	QRectF r(rect().normalized());

	if(r.width() < minSize)
		r.setWidth(minSize);
	if(r.height() < minSize)
		r.setHeight(minSize);

	setRect(r);
}
