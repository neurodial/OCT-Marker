#include "octmarkerio.h"

#include <helper/ptreehelper.h>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/info_parser.hpp>

#include <boost/lexical_cast.hpp>

#include <boost/filesystem.hpp>

#include "octdatamanager.h"

namespace bpt = boost::property_tree;
namespace bfs = boost::filesystem;

namespace
{
	namespace Constants
	{
		const char* mainNodeName = "OctMarker";
		const int   version      = 1;
	}
}


OctMarkerIO::OctMarkerIO(boost::property_tree::ptree* markerTree, const OctDataManager* dataManager)
: markerstree(markerTree)
, octDataManager(dataManager)
{
	
}

int OctMarkerIO::fileformat2Int(OctMarkerFileformat format)
{
	switch(format)
	{
		case OctMarkerFileformat::XML:
			return static_cast<int>(OctMarkerFileformat::XML);
		case OctMarkerFileformat::Josn:
			return static_cast<int>(OctMarkerFileformat::Josn);
		case OctMarkerFileformat::INFO:
			return static_cast<int>(OctMarkerFileformat::INFO);
		case OctMarkerFileformat::Unknown:
		case OctMarkerFileformat::Auto:
			break;
	}
	return -1;
}

OctMarkerFileformat OctMarkerIO::int2Fileformat(int formatId)
{
	switch(formatId)
	{
		case static_cast<int>(OctMarkerFileformat::XML):
			return OctMarkerFileformat::XML;
		case static_cast<int>(OctMarkerFileformat::Josn):
			return OctMarkerFileformat::Josn;
		case static_cast<int>(OctMarkerFileformat::INFO):
			return OctMarkerFileformat::INFO;
	}
	return OctMarkerFileformat::Unknown;
}



OctMarkerFileformat OctMarkerIO::getFormatFromExtension(const std::string& filename)
{

	bfs::path file(filename);
	std::string extension = file.extension().generic_string();
	
	if(extension == getFileExtension(OctMarkerFileformat::Josn))
		return OctMarkerFileformat::Josn;
	if(extension == getFileExtension(OctMarkerFileformat::XML))
		return OctMarkerFileformat::XML;
	if(extension == getFileExtension(OctMarkerFileformat::INFO))
		return OctMarkerFileformat::INFO;
	
	return OctMarkerFileformat::Unknown;
}


const char* OctMarkerIO::getFileExtension(OctMarkerFileformat format)
{
	switch(format)
	{
		case OctMarkerFileformat::Josn:
			return "joctmarker";
		case OctMarkerFileformat::XML:
			return "xoctmarker";
		case OctMarkerFileformat::INFO:
			return "ioctmarker";
		case OctMarkerFileformat::Unknown:
		case OctMarkerFileformat::Auto:
			break;
	}
	return "";
}

std::string OctMarkerIO::addMarkerExtension(const std::string& file, OctMarkerFileformat format)
{
	return file + "." + getFileExtension(format);
}



bool OctMarkerIO::loadDefaultMarker()
{
	std::string octFilename = octDataManager->getLoadedFilename().toStdString();
	
	OctMarkerFileformat formats[] = { OctMarkerFileformat::Josn,
	                                  OctMarkerFileformat::XML,
	                                  OctMarkerFileformat::INFO };
	
	for(std::size_t i = 0; i < sizeof(formats)/sizeof(formats[0]); ++i)
	{
		bfs::path markersFile = addMarkerExtension(octFilename, formats[i]);
		if(bfs::exists(markersFile))
		{
			defaultLoadedFormat = formats[i];
			return loadMarkers(markersFile.generic_string(), defaultLoadedFormat);
		}
	}
	
	return false;
}


bool OctMarkerIO::saveDefaultMarker()
{
	return saveMarkers(addMarkerExtension(octDataManager->getLoadedFilename().toStdString(), defaultLoadedFormat), defaultLoadedFormat);
}


bool OctMarkerIO::loadMarkers(const std::string& markersFilename, OctMarkerFileformat format)
{
	if(format == OctMarkerFileformat::Auto)
		format = getFormatFromExtension(markersFilename);
	if(format == OctMarkerFileformat::Unknown)
		return false;
	
	bpt::ptree loadTree;

	switch(format)
	{
		case OctMarkerFileformat::Josn:
			bpt::read_json(markersFilename, loadTree);
			break;
		case OctMarkerFileformat::XML:
			bpt::read_xml(markersFilename, loadTree, bpt::xml_parser::trim_whitespace);
			break;
		case OctMarkerFileformat::INFO:
			bpt::read_info(markersFilename, loadTree);
			break;
		case OctMarkerFileformat::Auto:
		case OctMarkerFileformat::Unknown:
			return false;
	}

	boost::optional<bpt::ptree&> nodeMain = loadTree.get_child_optional(Constants::mainNodeName);
	if(!nodeMain)
		return false;

	boost::optional<bpt::ptree&> nodeVersion = nodeMain->get_child_optional("Version");
	if(!nodeVersion)
		return false;

	if(nodeVersion->get_value<int>(0) != Constants::version)
		return false;


	boost::optional<bpt::ptree&> nodeMarkers = nodeMain->get_child_optional("Markers");
	if(!nodeMarkers)
		return false;

	*markerstree = *nodeMarkers;
	
	return true;
}

bool OctMarkerIO::saveMarkers(const std::string& markersFilename, OctMarkerFileformat format)
{
	if(format == OctMarkerFileformat::Auto)
		format = getFormatFromExtension(markersFilename);
	if(format == OctMarkerFileformat::Unknown)
		return false;
	
	bpt::ptree saveTree;
	bpt::ptree& markerTree = saveTree.put(Constants::mainNodeName, "");
	markerTree.put("Version", Constants::version);
    markerTree.add_child("Markers", *markerstree);
	
	
	switch(format)
	{
		case OctMarkerFileformat::Josn:
			bpt::write_json(markersFilename, saveTree);
			break;
		case OctMarkerFileformat::XML:
			bpt::write_xml(markersFilename, saveTree, std::locale(), bpt::xml_writer_make_settings<bpt::ptree::key_type>('\t', 1u));
			break;
		case OctMarkerFileformat::INFO:
			bpt::write_info(markersFilename, saveTree, std::locale(), bpt::info_writer_settings<char>('\t', 1u));
			break;
		case OctMarkerFileformat::Unknown:
		case OctMarkerFileformat::Auto:
			return false;
	}
	
	return true;
}

