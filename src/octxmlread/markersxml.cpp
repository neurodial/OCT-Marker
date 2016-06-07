#include "markersxml.h"

#include <iostream>

#include <manager/markermanager.h>
#include <data_structure/cscan.h>


#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

#include <boost/filesystem.hpp>

#include <boost/lexical_cast.hpp>


using boost::property_tree::ptree;
using boost::property_tree::xml_parser::read_xml;

namespace bfs = boost::filesystem;

void MarkersXML::readXML(MarkerManager* markerManger, std::string filename)
{
	if(!markerManger)
		return;
	
	if(!bfs::exists(filename))
		return;
	
	ptree xmltree;
	
	read_xml(filename, xmltree);
	
	
	const char* bscansNodeStr = "OCT";
	ptree& bscansNode = xmltree.get_child(bscansNodeStr);

	for(const std::pair<const std::string, ptree>& bscanPair : bscansNode)
	{
		if(bscanPair.first != "BScan")
			continue;

		const ptree& bscanNode = bscanPair.second;
		int bscanId = bscanNode.get_child("ID").get_value<int>();
		
		
		boost::optional<const ptree&> quallityIntervallsNode = bscanNode.get_child_optional("QuallityIntervalls");
		
		if(quallityIntervallsNode)
		{
			for(const std::pair<const std::string, ptree>& intervallNodePair : *quallityIntervallsNode)
			{
				if(intervallNodePair.first != "Intervall")
					continue;
				
				const ptree& intervallNode = intervallNodePair.second;
				
				int start          = intervallNode.get_child("Start").get_value<int>();
				int end            = intervallNode.get_child("End"  ).get_value<int>();
				int intervallClass = intervallNode.get_child("Class").get_value<int>();
				
				markerManger->setMarker(start, end, intervallClass, bscanId);
			}
		}

		
	}

	
}

void MarkersXML::writeXML(MarkerManager* markerManger, std::string filename)
{
	if(!markerManger)
		return;
	
	ptree xmltree;
	
	int numBscans = markerManger->getCScan().bscanCount();
	for(int bscan = 0; bscan < numBscans; ++bscan)
	{
		std::string nodeName = "OCT.BScan";
		ptree& bscanNode = xmltree.add(nodeName, "");
		bscanNode.add("ID", boost::lexical_cast<std::string>(bscan));
		
		for(const MarkerManager::MarkerMap::interval_mapping_type pair : markerManger->getMarkers(bscan))
		{
			
			// std::cout << "paintEvent(QPaintEvent* event) " << pair.second << " - " << pair.first << std::endl;

			int markerQ = pair.second;
			if(markerQ >= 0)
			{
				boost::icl::discrete_interval<int> itv  = pair.first;
				
				ptree& intervallNode = bscanNode.add("QuallityIntervalls.Intervall", "");
				
				intervallNode.add("Start", itv.lower());
				intervallNode.add("End"  , itv.upper());
				intervallNode.add("Class", markerQ);
				
				// xmltree.add(name, value);
			}
		}
	}
	
    boost::property_tree::xml_writer_settings<char> settings('\t', 1);
    write_xml(filename, xmltree, std::locale(), settings);
}

