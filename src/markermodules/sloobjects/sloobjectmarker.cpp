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

#include "sloobjectmarker.h"

#include "sloobjectsptree.h"

#include <QGraphicsScene>

#include <markerobjects/rectitem.h>

SloObjectMarker::SloObjectMarker(OctMarkerManager* markerManager)
: SloMarkerBase(markerManager)
, graphicsScene(new QGraphicsScene(this))
{
	name = tr("Slo objects");
	id   = "SloObjects";
	icon = QIcon(":/icons/tag_blue_edit.png");


	RectItem* onhMarker = new RectItem();
	onhMarker->setDescription("ONH");
	graphicsScene->addItem(onhMarker);
	rectItems["ONH"] = onhMarker;


	QPen pen1;
	pen1.setWidth(1);
	pen1.setCosmetic(true);
	onhMarker->setPen(pen1);

	resetScene();

}

SloObjectMarker::~SloObjectMarker()
{
	delete graphicsScene;
}

void SloObjectMarker::resetScene()
{
	const double scaleFactor = getScaleFactor();

	RectItem* onhMarker = rectItems["ONH"];
	onhMarker->setRect(QRectF(0.25*scaleFactor, 0.25*scaleFactor, 0.50*scaleFactor, 0.50*scaleFactor));
}

RectItem* SloObjectMarker::getRectItem(const std::string& id)
{
	RectItems::iterator it = rectItems.find(id);
	if(it == rectItems.end())
		return nullptr;
	return it->second;
}

const RectItem* SloObjectMarker::getRectItem(const std::string& id) const
{
	RectItems::const_iterator it = rectItems.find(id);
	if(it == rectItems.end())
		return nullptr;
	return it->second;
}


void SloObjectMarker::loadState(boost::property_tree::ptree& markerTree)
{
	resetScene();
	SloObjectsPtree::parsePTree(markerTree, this);
}

void SloObjectMarker::saveState(boost::property_tree::ptree& markerTree)
{
	SloObjectsPtree::fillPTree(markerTree, this);

}

