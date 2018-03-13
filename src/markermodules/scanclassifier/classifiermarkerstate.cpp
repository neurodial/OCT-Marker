#include "classifiermarkerstate.h"

ClassifierMarkerState::ClassifierMarkerState(const ClassifierMarker& marker)
: cassifierMarker(&marker)
{
// 	if(marker.getclassifierChoiceType() == ClassifierMarker::ClassifierChoiceType::Multible)
		markerStatus.resize(marker.size());

}

void ClassifierMarkerState::reset()
{
	std::size_t vSize = markerStatus.size();
	markerStatus.clear();
	markerStatus.resize(vSize);
}


void ClassifierMarkerState::fillPTree(boost::property_tree::ptree& ptree)
{
}

bool ClassifierMarkerState::parsePTree(const boost::property_tree::ptree& ptree)
{
}
