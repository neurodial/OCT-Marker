#include "objectsmarkerscene.h"

#include<QKeyEvent>


#include <markerobjects/rectitem.h>
#include <QGraphicsSceneMouseEvent>


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
		newaddedItem = new RectItem();
		newaddedItem->setRect(QRectF(event->scenePos(), QSizeF(0,0)));
		newaddedItem->setSelected(true);


		QPen pen1;
		pen1.setWidth(1);
		pen1.setCosmetic(true);
		newaddedItem->setPen(pen1);

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
	addObjectMode = v;
	if(!v && newaddedItem)
	{
		removeItem(newaddedItem);
		delete newaddedItem;
	}
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

