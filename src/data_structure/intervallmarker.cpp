#include "intervallmarker.h"

IntervallMarker::IntervallMarker()
{
	markerList.push_back(Marker("undef",   0,   0,   0));
	markerList.push_back(Marker("good" ,   0, 255,   0));
	markerList.push_back(Marker("mid"  , 255, 255,   0));
	markerList.push_back(Marker("bad"  , 255,   0,   0));
}


IntervallMarker::Marker::Marker(const std::string& name, uint8_t red, uint8_t green, uint8_t blue)
: name(name)
{
	color.red   = red  ;
	color.green = green;
	color.blue  = blue ;

}
