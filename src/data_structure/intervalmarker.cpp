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
	if(id>=0 && id < static_cast<int>(markerList.size()))
		return markerList[id];
	return markerList[0]; // TODO: besserer Rückgabewert
}

void IntervalMarker::addMarker(const IntervalMarker::Marker& marker)
{
	markerList.push_back(marker);
}

