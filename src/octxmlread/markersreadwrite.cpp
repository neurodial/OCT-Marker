#include "markersreadwrite.h"

#include <iostream>

#include <manager/bscanmarkermanager.h>
#include <octdata/datastruct/series.h>


#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/property_tree/json_parser.hpp>

#include <boost/filesystem.hpp>

#include <boost/lexical_cast.hpp>


namespace bpt = boost::property_tree;
namespace bfs = boost::filesystem;

namespace
{
	bool parsePTree(const bpt::ptree& ptree, BScanMarkerManager* markerManager)
	{
		const char* bscansNodeStr = "OCT";
		boost::optional<const bpt::ptree&> bscansNode = ptree.get_child_optional(bscansNodeStr);

		if(!bscansNode)
			return false;


		for(const std::pair<const std::string, const bpt::ptree>& bscanPair : *bscansNode)
		{
			if(bscanPair.first != "BScan")
				continue;

			const bpt::ptree& bscanNode = bscanPair.second;
			int bscanId = bscanNode.get_child("ID").get_value<int>();


			boost::optional<const bpt::ptree&> quallityIntervallsNode = bscanNode.get_child_optional("QuallityIntervalls");

			if(quallityIntervallsNode)
			{
				for(const std::pair<const std::string, const bpt::ptree>& intervallNodePair : *quallityIntervallsNode)
				{
					if(intervallNodePair.first != "Intervall")
						continue;

					const bpt::ptree& intervallNode = intervallNodePair.second;

					int         start          = intervallNode.get_child("Start").get_value<int>();
					int         end            = intervallNode.get_child("End"  ).get_value<int>();
					std::string intervallClass = intervallNode.get_child("Class").get_value<std::string>();

					try
					{
						IntervallMarker::Marker marker = IntervallMarker::getInstance().getMarkerFromString(intervallClass);
						markerManager->setMarker(start, end, marker, bscanId);
					}
					catch(std::out_of_range& r)
					{
						std::cerr << "unknown intervall class " << intervallClass << " : " << r.what() << std::endl;
					}

				}
			}
		}
		return true;
	}

	void fillPTree(bpt::ptree& markerTree, const BScanMarkerManager* markerManager)
	{

		int numBscans = markerManager->getSeries().bscanCount();
		for(int bscan = 0; bscan < numBscans; ++bscan)
		{
			std::string nodeName = "OCT.BScan";
			bpt::ptree& bscanNode = markerTree.add(nodeName, "");
			bscanNode.add("ID", boost::lexical_cast<std::string>(bscan));

			for(const BScanMarkerManager::MarkerMap::interval_mapping_type pair : markerManager->getMarkers(bscan))
			{

				// std::cout << "paintEvent(QPaintEvent* event) " << pair.second << " - " << pair.first << std::endl;

				IntervallMarker::Marker marker = pair.second;
				if(marker.isDefined())
				{
					boost::icl::discrete_interval<int> itv  = pair.first;

					bpt::ptree& intervallNode = bscanNode.add("QuallityIntervalls.Intervall", "");

					intervallNode.add("Start", itv.lower());
					intervallNode.add("End"  , itv.upper());
					intervallNode.add("Class", marker.getName());

					// xmltree.add(name, value);
				}
			}
		}
	}
}

bool MarkersReadWrite::readXML(BScanMarkerManager* markerManger, std::string filename)
{
	if(!markerManger)
		return false;
	
	if(!bfs::exists(filename))
		return false;
	
	bpt::ptree xmltree;
	bpt::read_xml(filename, xmltree);
	
	return parsePTree(xmltree, markerManger);
}

bool MarkersReadWrite::readJosn(BScanMarkerManager* markerManger, std::string filename)
{
	if(!markerManger)
		return false;

	if(!bfs::exists(filename))
		return false;

	bpt::ptree xmltree;
	bpt::read_json(filename, xmltree);

	return parsePTree(xmltree, markerManger);
}

void MarkersReadWrite::writeJosn(BScanMarkerManager* markerManger, std::string filename)
{
	if(!markerManger)
		return;

	bpt::ptree xmltree;
	fillPTree(xmltree, markerManger);

	bpt::write_json(filename, xmltree);
}


void MarkersReadWrite::writeXML(BScanMarkerManager* markerManger, std::string filename)
{
	if(!markerManger)
		return;
	
	bpt::ptree xmltree;

	fillPTree(xmltree, markerManger);

// #pragma message(__GNUC__);
//
// #if __GNUC__ <= 4 && __GNUC__ > 0
// 	bpt::xml_writer_settings<char> settings('\t', 1);
// 	bpt::write_xml(filename, xmltree, std::locale(), settings);
// #else
	bpt::write_xml(filename, xmltree, std::locale(), bpt::xml_writer_make_settings<bpt::ptree::key_type>('\t', 1u));
// #endif
}

