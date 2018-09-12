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

#include "objectsmarkerscene.h"

#include<QKeyEvent>


#include <markerobjects/rectitem.h>
#include <QGraphicsSceneMouseEvent>

#include"objectsmarkerfactory.h"

ObjectsmarkerScene::~ObjectsmarkerScene()
{

}

void ObjectsmarkerScene::keyPressEvent(QKeyEvent* event)
{

	QGraphicsScene::keyPressEvent(event);

}

void ObjectsmarkerScene::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
	if(addObjectMode)
	{
		endInsertItem();
		newaddedItem = factory.createObject();
		newaddedItem->setRect(QRectF(event->scenePos(), QSizeF(0,0)));
		newaddedItem->setSelected(true);

		addItem(newaddedItem);
	}
	else
		QGraphicsScene::mousePressEvent(event);
}


void ObjectsmarkerScene::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
	if(newaddedItem)
	{
		QRectF r = newaddedItem->rect();
		r.setTopLeft(event->scenePos());
		newaddedItem->setRect(r);
	}
	else
		QGraphicsScene::mouseMoveEvent(event);
}


void ObjectsmarkerScene::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
	if(newaddedItem)
	{
		endInsertItem();
		addObjectMode = false;
		addObjectModeChanged(addObjectMode);
	}
	else
		QGraphicsScene::mouseReleaseEvent(event);
}


void ObjectsmarkerScene::endInsertItem()
{
	if(newaddedItem)
	{
		if(newaddedItem->rect().size().isNull())
		{
			removeItem(newaddedItem);
			delete newaddedItem;
		}
		else
			newaddedItem->makeValid();
		newaddedItem = nullptr;
	}
}


void ObjectsmarkerScene::setAddObjectMode(bool v)
{
	if(v == addObjectMode)
		return;

	addObjectMode = v;
	if(!v && newaddedItem)
	{
		removeItem(newaddedItem);
		delete newaddedItem;
		newaddedItem = nullptr;
	}
	addObjectModeChanged(addObjectMode);
}


void ObjectsmarkerScene::markersFromList(std::vector<RectItem*>& itemslist)
{
	for(RectItem* item : itemslist)
		addItem(item);
	itemslist.clear();
}


void ObjectsmarkerScene::markersToList(std::vector<RectItem*>& itemslist)
{
	endInsertItem();
	for(QGraphicsItem* item : items())
	{
		RectItem* rectItem = dynamic_cast<RectItem*>(item);
		if(rectItem)
		{
			itemslist.push_back(rectItem);
			removeItem(rectItem);
		}
	}
}

