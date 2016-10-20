#include "octmarkerdata.h"


#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/property_tree/json_parser.hpp>

#include <boost/filesystem.hpp>

#include <boost/lexical_cast.hpp>


namespace bpt = boost::property_tree;
namespace bfs = boost::filesystem;


OctMarkerData::OctMarkerData()
: ptree(new bpt::ptree)
{
}


OctMarkerData::~OctMarkerData()
{
	delete ptree;
}

