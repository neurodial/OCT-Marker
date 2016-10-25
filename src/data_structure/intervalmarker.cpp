#include "intervalmarker.h"


#include <stdexcept>

std::size_t IntervalMarker::Marker::markerCounter = 0;


IntervalMarker::IntervalMarker()
{
	markerList.push_back(Marker());
	markerList.push_back(Marker("good" ,   0, 255,   0));
	markerList.push_back(Marker("mid"  , 255, 255,   0));
	markerList.push_back(Marker("bad"  , 255,   0,   0));
}

IntervalMarker::Marker::Marker()
: internalId(0)
, name("undefined")
, defined(false)
{
	color.red   = 0;
	color.green = 0;
	color.blue  = 0;
}


IntervalMarker::Marker::Marker(const std::string& name, uint8_t red, uint8_t green, uint8_t blue)
: internalId(++markerCounter)
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
		if(marker.getName() == str)
			return marker;

	throw(std::out_of_range(str + " is not in marker list"));
}

const IntervalMarker::Marker& IntervalMarker::getMarkerFromID(int id) const
{
	if(id>=0 && id < static_cast<int>(markerList.size()))
		return markerList[id];
	return markerList[0]; // TODO: besserer Rückgabewert
}
