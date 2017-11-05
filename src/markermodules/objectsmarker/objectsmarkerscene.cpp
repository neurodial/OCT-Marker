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
	if(addObjectMode && newaddedItem)
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
	if(addObjectMode)
	{
		if(newaddedItem->rect().size().isNull())
		{
			removeItem(newaddedItem);
			delete newaddedItem;
		}
		newaddedItem = nullptr;
		addObjectMode = false;
	}
	else
		QGraphicsScene::mouseReleaseEvent(event);

}
