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

#include "bscanlayersegptree.h"

#include"bscanlayersegmentation.h"

#include<sstream>
#include<iostream>

#include <boost/property_tree/ptree.hpp>
#include <boost/lexical_cast.hpp>

namespace bpt = boost::property_tree;

#include<boost/spirit/include/qi.hpp>
namespace qi = boost::spirit::qi;

#include<octdata/datastruct/segmentationlines.h>
#include <helper/ptreehelper.h>




namespace
{
	bool emptySegLine(const std::vector<double>& vec)
	{
		for(double val : vec)
			if(val != 0 && val < 1e8)
				return false;
		return true;
	}

	template<typename T>
	std::vector<T> fillToVector(const bpt::ptree& pt)
	{
		std::vector<T> r;

		boost::optional<std::string> s = pt.get_value_optional<std::string>();
		if(s)
		{
			std::string::const_iterator f(s->begin()), l(s->end());
			/*bool ok = */qi::parse(f,l,qi::double_ % ' ',r);
		}
	    return r;
	}

	template<typename T>
	void fillFromVector(bpt::ptree& pt, const std::vector<T>& vec)
	{
		pt.clear();

		std::stringstream sstream;
		for(const T& val : vec)
			sstream << val << ' ';

		pt.put_value(sstream.str());
	}

}


void BScanLayerSegPTree::fillPTree(boost::property_tree::ptree& ptree, const BScanLayerSegmentation* markerManager)
{
	ptree.clear(); // TODO

	std::size_t bscan = 0;
	for(const BScanLayerSegmentation::BScanSegData& bscanData : markerManager->lines)
	{
		const OctData::Segmentationlines& lines = bscanData.lines;

		PTreeHelper::NodeCreator bscanNode("BScan", ptree);
		bscanNode.setId(bscan);
		PTreeHelper::NodeCreator linesNode("Lines", bscanNode);

		for(OctData::Segmentationlines::SegmentlineType type : OctData::Segmentationlines::getSegmentlineTypes())
		{
			bool isLoaded  = bscanData.lineLoaded  [static_cast<std::size_t>(type)];
			bool isModifed = bscanData.lineModified[static_cast<std::size_t>(type)];

			if(!isLoaded && !isModifed)
				continue;

			const OctData::Segmentationlines::Segmentline& line = lines.getSegmentLine(type);
			const char* name = lines.getSegmentlineName(type);

			if(!emptySegLine(line))
			{
				bpt::ptree& lineNode = PTreeHelper::get_put(linesNode.getNode(), name);
				fillFromVector(lineNode, line);
			}
		}

		++bscan;
	}
}

bool BScanLayerSegPTree::parsePTree(const boost::property_tree::ptree& ptree, BScanLayerSegmentation* markerManager)
{

	for(const std::pair<const std::string, const bpt::ptree>& bscanPair : ptree)
	{
		if(bscanPair.first != "BScan")
			continue;

		const bpt::ptree& bscanNode = bscanPair.second;
		boost::optional<const bpt::ptree&> idNode = bscanNode.get_child_optional("ID");
		if(!idNode)
			continue;

		int bscanId = idNode->get_value<int>(-1);
		if(bscanId < 0)
			continue;
		if(bscanId >= markerManager->lines.size())
			continue;

		boost::optional<const bpt::ptree&> linesNode = bscanNode.get_child_optional("Lines");
		if(!linesNode)
			continue;

		BScanLayerSegmentation::BScanSegData& bscanData = markerManager->lines[bscanId];

		for(const std::pair<const std::string, const bpt::ptree>& segLinesNodePair : *linesNode)
		{
			const std::string& name = segLinesNodePair.first;

			OctData::Segmentationlines::SegmentlineType actType;
			bool found = false;

			const OctData::Segmentationlines::SegLinesTypeList& seglineTypes = OctData::Segmentationlines::getSegmentlineTypes();
			for(OctData::Segmentationlines::SegmentlineType type : seglineTypes)
			{
				if(OctData::Segmentationlines::getSegmentlineName(type) == name)
				{
					actType = type;
					found = true;
					break;
				}
			}

			if(!found)
			{
				std::cerr << "unhandled segmentation line: " << name << '\n';
				continue;
			}

			bscanData.lines.getSegmentLine(actType) = fillToVector<double>(segLinesNodePair.second);
			bscanData.lineLoaded[static_cast<std::size_t>(actType)] = true;
		}
	}

	return true;
}
