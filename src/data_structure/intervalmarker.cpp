#include "intervalmarker.h"


#include <stdexcept>

std::size_t IntervalMarker::Marker::markerCounter = 0;


IntervalMarker::IntervalMarker(const std::string& internalName, const std::string& viewName)
: internalName(internalName)
, viewName    (viewName    )
{
	markerList.push_back(Marker());

/*
	markerList.push_back(Marker("good"          , "good signal"             ,   0, 255,   0));
	markerList.push_back(Marker("mid"           , "middle signal"           , 255, 255,   0));
	markerList.push_back(Marker("bad"           , "bad signal"              , 255,   0,   0));

	markerList.push_back(Marker("signalmissing" , "signal missing"          ,   0, 140, 140));
	markerList.push_back(Marker("ILM_upper"     , "ILM upper"               ,   0, 205, 205));
	markerList.push_back(Marker("ILM_lower"     , "ILM lower"               , 255, 255,   0));
	markerList.push_back(Marker("BM_upper"      , "BM upper"                , 240, 110,  90));
	markerList.push_back(Marker("BM_lower"      , "BM lower"                , 255, 140,   0));
	markerList.push_back(Marker("SegILMmissing" , "missing ILM segmentation", 255,   0,   0));
	markerList.push_back(Marker("SegBMmissing"  , "missing BM segmentation" , 255, 200,   0));
	markerList.push_back(Marker("MultiError"    , "multi error"             , 138,  43, 226));*/
}

IntervalMarker::Marker::Marker()
: internalId(0)
, internalName("undefined")
, name("undefined")
, defined(false)
{
	color.red   = 0;
	color.green = 0;
	color.blue  = 0;
}


IntervalMarker::Marker::Marker(const std::string& internalName, const std::string& name, uint8_t red, uint8_t green, uint8_t blue)
: internalId(++markerCounter)
, internalName(internalName)
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

