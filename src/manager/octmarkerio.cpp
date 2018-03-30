#include "octmarkerio.h"

#ifndef MEX_COMPILE
#include <data_structure/programoptions.h>
#endif

#include <helper/ptreehelper.h>
#include <cpp_framework/platform_helper/filename_unicode.h>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/info_parser.hpp>

#include <boost/iostreams/device/file_descriptor.hpp>
#include <boost/iostreams/stream.hpp>
#include <boost/filesystem.hpp>
#include <iostream>

namespace pt = boost::property_tree;
namespace io = boost::iostreams;
namespace fs = boost::filesystem;


#include <boost/lexical_cast.hpp>

#include <boost/filesystem.hpp>


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


OctMarkerIO::OctMarkerIO(boost::property_tree::ptree* markerTree)
: markerstree(markerTree)
{
	
}

OctMarkerFileformat OctMarkerIO::getDefaultFileFormat()
{
#ifndef MEX_COMPILE
	OctMarkerFileformat format = int2Fileformat(ProgramOptions::defaultFileformatOctMarkers());
#else
	OctMarkerFileformat format = OctMarkerFileformat::INFO;
#endif

	switch(format)
	{
		case OctMarkerFileformat::XML:
		case OctMarkerFileformat::Json:
		case OctMarkerFileformat::INFO:
			return format;
		case OctMarkerFileformat::Unknown:
		case OctMarkerFileformat::Auto:
		case OctMarkerFileformat::NoExtension:
			break;
	}
	return OctMarkerFileformat::Json;
}


int OctMarkerIO::fileformat2Int(OctMarkerFileformat format)
{
	switch(format)
	{
		case OctMarkerFileformat::XML:
			return static_cast<int>(OctMarkerFileformat::XML);
		case OctMarkerFileformat::Json:
			return static_cast<int>(OctMarkerFileformat::Json);
		case OctMarkerFileformat::INFO:
			return static_cast<int>(OctMarkerFileformat::INFO);
		case OctMarkerFileformat::Unknown:
		case OctMarkerFileformat::Auto:
		case OctMarkerFileformat::NoExtension:
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
		case static_cast<int>(OctMarkerFileformat::Json):
			return OctMarkerFileformat::Json;
		case static_cast<int>(OctMarkerFileformat::INFO):
			return OctMarkerFileformat::INFO;
	}
	return OctMarkerFileformat::Unknown;
}



OctMarkerFileformat OctMarkerIO::getFormatFromExtension(const std::string& filename)
{
	bfs::path markersPath(filename);
	return getFormatFromExtension(markersPath);
}

OctMarkerFileformat OctMarkerIO::getFormatFromExtension(const boost::filesystem::path& markersPath)
{
	std::string extension = markersPath.extension().generic_string();
	if(extension.length() == 0)
		return OctMarkerFileformat::NoExtension;

	extension = extension.substr(1);

	if(extension == getFileExtension(OctMarkerFileformat::Json))
		return OctMarkerFileformat::Json;
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
		case OctMarkerFileformat::Json:
			return "joctmarker";
		case OctMarkerFileformat::XML:
			return "xoctmarker";
		case OctMarkerFileformat::INFO:
			return "ioctmarker";
		case OctMarkerFileformat::Unknown:
		case OctMarkerFileformat::Auto:
		case OctMarkerFileformat::NoExtension:
			break;
	}
	return "";
}

std::string OctMarkerIO::addMarkerExtension(const std::string& file, OctMarkerFileformat format)
{
	return file + "." + getFileExtension(format);
}



bool OctMarkerIO::loadDefaultMarker(const std::string& octFilename)
{
	loadedDefaultFilename.clear();
	OctMarkerFileformat formats[] = { OctMarkerFileformat::Json,
	                                  OctMarkerFileformat::XML,
	                                  OctMarkerFileformat::INFO };
	
	for(std::size_t i = 0; i < sizeof(formats)/sizeof(formats[0]); ++i)
	{
		bfs::path markersFile = filenameConv(addMarkerExtension(octFilename, formats[i]));
		if(bfs::exists(markersFile))
		{
			defaultLoadedFormat = formats[i];
			loadedDefaultFilename = markersFile.generic_string();
			return loadMarkers(markersFile, defaultLoadedFormat);
		}
	}


	if(octFilename.substr(octFilename.size()-3, 3) == ".gz")
		return loadDefaultMarker(octFilename.substr(0, octFilename.size()-3));

	defaultLoadedFormat = getDefaultFileFormat();
	return false;
}


bool OctMarkerIO::saveDefaultMarker(const std::string& octFilename)
{
	if(loadedDefaultFilename.empty())
		return saveMarkers(addMarkerExtension(octFilename, defaultLoadedFormat), defaultLoadedFormat);
	else
		return saveMarkers(loadedDefaultFilename, defaultLoadedFormat);
}

bool OctMarkerIO::loadMarkers(const boost::filesystem::path& markersPath, OctMarkerFileformat format)
{
	if(format == OctMarkerFileformat::Auto)
		format = getFormatFromExtension(markersPath);
	if(format == OctMarkerFileformat::Unknown
	|| format == OctMarkerFileformat::NoExtension)
		return false;

	if(!bfs::exists(markersPath))
		return false;

	bpt::ptree loadTree;

    io::file_descriptor_source fs(markersPath);
    io::stream<io::file_descriptor_source> fsstream(fs);

	switch(format)
	{
		case OctMarkerFileformat::Json:
			bpt::read_json(fsstream, loadTree);
			break;
		case OctMarkerFileformat::XML:
			bpt::read_xml(fsstream, loadTree, bpt::xml_parser::trim_whitespace);
			break;
		case OctMarkerFileformat::INFO:
			bpt::read_info(fsstream, loadTree);
			break;
		case OctMarkerFileformat::Auto: // avoid compiler warnings
		case OctMarkerFileformat::Unknown:
		case OctMarkerFileformat::NoExtension:
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



bool OctMarkerIO::loadMarkers(const std::string& markersFilename, OctMarkerFileformat format)
{
	boost::filesystem::path markersPath(filenameConv(markersFilename));
	return loadMarkers(markersPath, format);
}

bool OctMarkerIO::saveMarkers(const std::string& markersFilename, OctMarkerFileformat format)
{
	if(format == OctMarkerFileformat::Auto)
		format = getFormatFromExtension(markersFilename);
	if(format == OctMarkerFileformat::NoExtension)
	{
		OctMarkerFileformat defaultFileFormat = getDefaultFileFormat();
		return saveMarkersPrivat(addMarkerExtension(markersFilename, defaultFileFormat), defaultFileFormat);
	}
	if(format == OctMarkerFileformat::Unknown)
		return false;
	
	return saveMarkersPrivat(markersFilename, format);
}


bool OctMarkerIO::saveMarkersPrivat(const std::string& markersFilename, OctMarkerFileformat format)
{
	bpt::ptree saveTree;
	bpt::ptree& markerTree = saveTree.put(Constants::mainNodeName, "");
	markerTree.put("Version", Constants::version);
    markerTree.add_child("Markers", *markerstree);


	boost::filesystem::path p(filenameConv(markersFilename));
    io::file_descriptor_sink fs(p);
    io::stream<io::file_descriptor_sink> fsstream(fs);


	switch(format)
	{
		case OctMarkerFileformat::Json:
			bpt::write_json(fsstream, saveTree);
			break;
		case OctMarkerFileformat::XML:
			bpt::write_xml(fsstream, saveTree, bpt::xml_writer_make_settings<bpt::ptree::key_type>('\t', 1u));
			break;
		case OctMarkerFileformat::INFO:
			bpt::write_info(fsstream, saveTree, bpt::info_writer_settings<char>('\t', 1u));
			break;
		case OctMarkerFileformat::Unknown:
		case OctMarkerFileformat::Auto:
		case OctMarkerFileformat::NoExtension:
			return false;
	}

	return true;
}

