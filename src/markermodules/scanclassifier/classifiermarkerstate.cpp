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
	markerStatus.resize(vSize, false);
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
		std::string tokenString = node->get<std::string>(""); // copy is needed
		boost::tokenizer<boost::char_separator<char>> tokens(tokenString, sep);

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
