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

#ifndef SLOOBJECTMARKER_H
#define SLOOBJECTMARKER_H

#include "../slomarkerbase.h"

#include <map>
#include <string>

class RectItem;

class SloObjectMarker : public SloMarkerBase
{
	Q_OBJECT
public:
	typedef std::map<std::string, RectItem*> RectItems;
	typedef RectItems::value_type RectItemsTypes;

	explicit SloObjectMarker(OctMarkerManager* markerManager);
	virtual ~SloObjectMarker();


	              QGraphicsScene* getGraphicsScene()       override { return graphicsScene; }
	virtual const QGraphicsScene* getGraphicsScene() const override { return graphicsScene; }

	virtual void newSeriesLoaded(const OctData::Series*, boost::property_tree::ptree& ptree) override
	                                                                { loadState(ptree); }

	virtual bool hasChangedSinceLastSave() const           override { return false; } // TODO: implement!

	virtual void saveState(boost::property_tree::ptree& markerTree)  override;
	virtual void loadState(boost::property_tree::ptree& markerTree)  override;

	      RectItem* getRectItem(const std::string& id);
	const RectItem* getRectItem(const std::string& id) const;

	const RectItems& getRectItems() const                           { return rectItems; }

private:
	QGraphicsScene* graphicsScene = nullptr;
	// RectItem*       sloMarker     = nullptr;

	RectItems rectItems;


	void resetScene();
};

#endif // SLOOBJECTMARKER_H
