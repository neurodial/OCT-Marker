#include "classifiermarker.h"

#include<algorithm>

ClassifierMarker::Marker::Marker(const std::string& internalName, const std::string& name)
: internalName(internalName)
, name        (name)
, id          (id)
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
