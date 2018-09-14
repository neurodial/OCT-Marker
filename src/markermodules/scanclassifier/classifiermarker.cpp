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

#include "classifiermarker.h"

#include<algorithm>

std::size_t ClassifierMarker::Marker::markerCounter = 0;


ClassifierMarker::Marker::Marker(const std::string& internalName, const std::string& name)
: internalName(internalName)
, name        (name)
, id          (markerCounter++)
{
}

ClassifierMarker::ClassifierMarker(const std::string& internalName, const std::string& viewName, ClassifierMarker::ClassifierChoiceType type)
: internalName(internalName)
, viewName(viewName)
, classifierChoiceType(type)
{
}

void ClassifierMarker::addMarker(const ClassifierMarker::Marker& marker)
{
	markerList.push_back(marker);
	markerList.back().id = markerList.size() - 1;
}

const ClassifierMarker::Marker& ClassifierMarker::getMarkerFromID(int id) const
{
	return markerList.at(id);
}

const ClassifierMarker::Marker* ClassifierMarker::getMarkerFromString(const std::string& str) const
{
	ClassifierMarkerList::const_iterator it = std::find(markerList.begin(), markerList.end(), str);
	if(it != markerList.end())
		return &(*it);
	return nullptr;
}
