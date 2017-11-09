#include "objectsmarkerfactory.h"


#include <markerobjects/rectitem.h>
#include<QPen>

RectItem* ObjectsmarkerFactory::createObject() const
{
	RectItem* newItem = new RectItem();

	QPen pen1;
	pen1.setWidth(1);
	pen1.setCosmetic(true);
	newItem->setPen(pen1);

	return newItem;
}
