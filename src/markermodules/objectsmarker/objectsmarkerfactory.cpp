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
