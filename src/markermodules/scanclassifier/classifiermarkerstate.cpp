#include "classifiermarkerstate.h"

#include<iostream>

#include <boost/tokenizer.hpp>

#include <boost/property_tree/ptree.hpp>
namespace bpt = boost::property_tree;

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


void ClassifierMarkerState::fillPTree(boost::property_tree::ptree& ptree) const
{
	ptree.erase(cassifierMarker->getInternalName());

	std::string token;
	for(std::size_t i = 0; i < markerStatus.size(); ++i)
	{
		if(markerStatus[i])
		{
			if(!token.empty())
				token += ";";
			token += cassifierMarker->getMarkerFromID(static_cast<int>(i)).getInternalName();
		}
	}

	if(!token.empty())
		ptree.put(cassifierMarker->getInternalName(), token);
}

bool ClassifierMarkerState::parsePTree(const boost::property_tree::ptree& ptree)
{


	boost::optional<const bpt::ptree&> node = ptree.get_child_optional(cassifierMarker->getInternalName());
	if(node)
	{
		boost::char_separator<char> sep(";");
		boost::tokenizer<boost::char_separator<char>> tokens(node->get<std::string>(""), sep);

		for(const std::string& token : tokens)
		{
			const ClassifierMarker::Marker* marker = cassifierMarker->getMarkerFromString(token);
			std::size_t id = marker->getId();
			if(id < markerStatus.size())
				markerStatus[id] = true;
			else
				std::cerr << __FUNCTION__ << ": unknown token: " << token << std::endl;
		}
		return true;
	}
	return false;
}
