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

	void loadItemState(const boost::property_tree::ptree& ptree, RectItem& item)
	{
		double centerPosX = readOptinalNode(ptree, "CenterPosX", 0.5)*100.;
		double centerPosY = readOptinalNode(ptree, "CenterPosY", 0.5)*100.;
		double height     = readOptinalNode(ptree, "Height"    , 0.5)*100.;
		double width      = readOptinalNode(ptree, "Width"     , 0.5)*100.;


		QPointF pos  = item.mapToScene(QPointF(centerPosX, centerPosY));
		QRectF  rect(pos.x()-width/2, pos.y()-height/2, width, height);
		item.setRect(rect);
	}

	void saveItemState(boost::property_tree::ptree& ptree, const RectItem& item)
	{
		QRectF rect = item.rect();

		QPointF pos = item.mapToScene(rect.center());

		ptree.put("ItemType"  , "Rect"       );
		ptree.put("CenterPosX", pos.x()      /100.);
		ptree.put("CenterPosY", pos.y()      /100.);
		ptree.put("Height"    , rect.height()/100.);
		ptree.put("Width"     , rect.width() /100.);
	}

}


void SloObjectsPtree::fillPTree(boost::property_tree::ptree& ptree, const SloObjectMarker* markerManager)
{
	boost::property_tree::ptree& ptreeObjects = PTreeHelper::get_put(ptree, "Objects");
	const SloObjectMarker::RectItems& rectItems = markerManager->getRectItems();
	for(const SloObjectMarker::RectItemsTypes& itemPair : rectItems)
	{
		if(itemPair.second)
		{
			boost::property_tree::ptree& ptreeRectItem = PTreeHelper::get_put(ptreeObjects, itemPair.first);
			saveItemState(ptreeRectItem, *(itemPair.second));
		}
	}
}


bool SloObjectsPtree::parsePTree(const boost::property_tree::ptree& ptree, SloObjectMarker* markerManager)
{
	boost::optional<const bpt::ptree&> ptreeObjects = ptree.get_child_optional("Objects");
	if(ptreeObjects)
	{
		for(const std::pair<const std::string, bpt::ptree>& child : *ptreeObjects)
		{
			const std::string& itemName = child.first;
			      std::string  itemType = readOptinalNode(child.second, "ItemType", std::string());

			if(itemType == "Rect")
			{
				RectItem* item = markerManager->getRectItem(itemName);
				if(item)
					loadItemState(child.second, *item);
			}
		}

		return true;
	}
	return false;
}
