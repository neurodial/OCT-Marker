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

#include "intervalmarker.h"


#include <stdexcept>

std::size_t IntervalMarker::Marker::markerCounter = 0;


IntervalMarker::IntervalMarker(const std::string& internalName, const std::string& viewName)
: internalName(internalName)
, viewName    (viewName    )
{
	markerList.push_back(Marker());
}

IntervalMarker::Marker::Marker()
: internalName("undefined")
, name("undefined")
, defined(false)
{
	color.red   = 0;
	color.green = 0;
	color.blue  = 0;
}


IntervalMarker::Marker::Marker(const std::string& internalName, const std::string& name, uint8_t red, uint8_t green, uint8_t blue)
// : internalId(++markerCounter)
: internalName(internalName)
, name(name)
, defined(true)
{
	color.red   = red  ;
	color.green = green;
	color.blue  = blue ;
}

const IntervalMarker::Marker& IntervalMarker::getMarkerFromString(const std::string& str) const
{
	for(const Marker& marker : markerList)
		if(marker.getInternalName() == str)
			return marker;

	throw(std::out_of_range(str + " is not in marker list"));
}

const IntervalMarker::Marker& IntervalMarker::getMarkerFromID(int id) const
{
	if(id>=0 && id < static_cast<int>(size()))
		return markerList[static_cast<std::size_t>(id)];
	throw std::out_of_range("invalid marker id: IntervalMarker::getMarkerFromID");
}

void IntervalMarker::addMarker(const IntervalMarker::Marker& marker)
{
	markerList.push_back(marker);
}

