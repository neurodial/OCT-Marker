#include "intervallmarker.h"


#include <stdexcept>

std::size_t IntervallMarker::Marker::markerCounter = 0;


IntervallMarker::IntervallMarker()
{
	markerList.push_back(Marker());
	markerList.push_back(Marker("good" ,   0, 255,   0));
	markerList.push_back(Marker("mid"  , 255, 255,   0));
	markerList.push_back(Marker("bad"  , 255,   0,   0));
}

IntervallMarker::Marker::Marker()
: internalId(0)
, name("undefined")
, defined(false)
{
	color.red   = 0;
	color.green = 0;
	color.blue  = 0;
}


IntervallMarker::Marker::Marker(const std::string& name, uint8_t red, uint8_t green, uint8_t blue)
: internalId(++markerCounter)
, name(name)
, defined(true)
{
	color.red   = red  ;
	color.green = green;
	color.blue  = blue ;
}

const IntervallMarker::Marker& IntervallMarker::getMarkerFromString(const std::string& str) const
{
	for(const Marker& marker : markerList)
		if(marker.getName() == str)
			return marker;

	throw(std::out_of_range(str + " is not in marker list"));
}

const IntervallMarker::Marker& IntervallMarker::getMarkerFromID(int id) const
{
	if(id>=0 && id < markerList.size())
		return markerList[id];
	return markerList[0]; // TODO: besserer Rückgabewert
}
