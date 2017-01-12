#ifndef OCTMARKERIO_H
#define OCTMARKERIO_H

#include<string>

#include<boost/property_tree/ptree_fwd.hpp>

#include<globaldefinitions.h>


class OctMarkerIO
{
	static OctMarkerFileformat getDefaultFileFormat();

	OctMarkerFileformat defaultLoadedFormat = OctMarkerFileformat::Json;
	
	boost::property_tree::ptree* markerstree = nullptr;

	bool saveMarkersPrivat(const std::string& markersFilename, OctMarkerFileformat format);
	
public:
	explicit OctMarkerIO(boost::property_tree::ptree* markerTree);
	
	
	static const char* getFileExtension(OctMarkerFileformat format);
	static OctMarkerFileformat getFormatFromExtension(const std::string& filename);
	static std::string addMarkerExtension(const std::string& file, OctMarkerFileformat format);
	
	static OctMarkerFileformat int2Fileformat(int formatId);
	static int fileformat2Int(OctMarkerFileformat format);
	
	
	bool saveDefaultMarker(const std::string& octFilename);
	bool loadDefaultMarker(const std::string& octFilename);
	
	bool loadMarkers(const std::string& markersFilename, OctMarkerFileformat format);
	bool saveMarkers(const std::string& markersFilename, OctMarkerFileformat format);
	
	bool saveMarkersSeries(const std::string& markersFilename);
	bool addMarkersSeries (const std::string& markersFilename);
};

#endif // OCTMARKERIO_H
