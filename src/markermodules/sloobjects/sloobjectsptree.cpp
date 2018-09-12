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

#include "sloobjectsptree.h"

#include "sloobjectmarker.h"
#include <helper/ptreehelper.h>
#include <markerobjects/rectitem.h>

#include <boost/property_tree/ptree.hpp>

namespace bpt = boost::property_tree;

#include <QGraphicsScene>
#include <QGraphicsItem>

namespace
{

	template<typename T>
	inline T readOptinalNode(const bpt::ptree& basis, const char* nodeName, T defaultValue)
	{
		boost::optional<const bpt::ptree&> optNode  = basis.get_child_optional(nodeName);
		if(optNode)
			return optNode->get_value<T>(defaultValue);
		return defaultValue;
	}

	void loadItemState(const boost::property_tree::ptree& ptree, RectItem& item, const double scaleFactor)
	{
		double centerPosX = readOptinalNode(ptree, "CenterPosX", 0.5)*scaleFactor;
		double centerPosY = readOptinalNode(ptree, "CenterPosY", 0.5)*scaleFactor;
		double height     = readOptinalNode(ptree, "Height"    , 0.5)*scaleFactor;
		double width      = readOptinalNode(ptree, "Width"     , 0.5)*scaleFactor;

		QPointF pos  = item.mapFromScene(QPointF(centerPosX, centerPosY));
		QRectF  rect(pos.x()-width/2, pos.y()-height/2, width, height);

		item.setRect(rect);
	}

	void saveItemState(boost::property_tree::ptree& ptree, const RectItem& item, const double scaleFactor)
	{
		QRectF rect = item.rect();

		QPointF pos = item.mapToScene(rect.center());

		ptree.put("ItemType"  , "Rect"       );
		ptree.put("CenterPosX", pos.x()      /scaleFactor);
		ptree.put("CenterPosY", pos.y()      /scaleFactor);
		ptree.put("Height"    , rect.height()/scaleFactor);
		ptree.put("Width"     , rect.width() /scaleFactor);
	}

}


void SloObjectsPtree::fillPTree(boost::property_tree::ptree& ptree, const SloObjectMarker* markerManager)
{
	const double scaleFactor = markerManager->getScaleFactor();

	boost::property_tree::ptree& ptreeObjects = PTreeHelper::get_put(ptree, "Objects");
	const SloObjectMarker::RectItems& rectItems = markerManager->getRectItems();
	for(const SloObjectMarker::RectItemsTypes& itemPair : rectItems)
	{
		if(itemPair.second)
		{
			boost::property_tree::ptree& ptreeRectItem = PTreeHelper::get_put(ptreeObjects, itemPair.first);
			saveItemState(ptreeRectItem, *(itemPair.second), scaleFactor);
		}
	}
}


bool SloObjectsPtree::parsePTree(const boost::property_tree::ptree& ptree, SloObjectMarker* markerManager)
{
	boost::optional<const bpt::ptree&> ptreeObjects = ptree.get_child_optional("Objects");
	if(ptreeObjects)
	{
		const double scaleFactor = markerManager->getScaleFactor();

		for(const std::pair<const std::string, bpt::ptree>& child : *ptreeObjects)
		{
			const std::string& itemName = child.first;
			      std::string  itemType = readOptinalNode(child.second, "ItemType", std::string());

			if(itemType == "Rect")
			{
				RectItem* item = markerManager->getRectItem(itemName);
				if(item)
					loadItemState(child.second, *item, scaleFactor);
			}
		}

		return true;
	}
	return false;
}
