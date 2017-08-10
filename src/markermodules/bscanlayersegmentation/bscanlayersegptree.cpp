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
		std::string nodeName = "BScan";
		bpt::ptree& bscanNode = ptree.add(nodeName, "");
		bscanNode.add("ID", boost::lexical_cast<std::string>(bscan));

		bpt::ptree& linesNode = bscanNode.add("Lines", "");

		for(OctData::Segmentationlines::SegmentlineType type : OctData::Segmentationlines::getSegmentlineTypes())
		{
			const OctData::Segmentationlines::Segmentline& line = lines.getSegmentLine(type);
			const char* name = lines.getSegmentlineName(type);

			if(!emptySegLine(line))
			{
				bpt::ptree& lineNode = PTreeHelper::get_put(linesNode, name);
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
			bscanData.filled = true;
		}
	}

	return true;
}
