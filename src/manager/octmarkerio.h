#ifndef OCTMARKERIO_H
#define OCTMARKERIO_H

#include<string>

#include<boost/property_tree/ptree_fwd.hpp>

#include<globaldefinitons.h>

// namespace boost{ namespace filesystem{ class path; }}

class OctDataManager;

class OctMarkerIO
{
	static const OctMarkerFileformat defaultFileFormat = OctMarkerFileformat::Josn;
	OctMarkerFileformat defaultLoadedFormat = OctMarkerFileformat::Josn;
	
	boost::property_tree::ptree* markerstree = nullptr;
	const OctDataManager* octDataManager = nullptr;
	
public:
	OctMarkerIO(boost::property_tree::ptree* markerTree, const OctDataManager* dataManager);
	
	
	static const char* getFileExtension(OctMarkerFileformat format);
	static OctMarkerFileformat getFormatFromExtension(const std::string& filename);
	static std::string addMarkerExtension(const std::string& file, OctMarkerFileformat format);
	
	static OctMarkerFileformat int2Fileformat(int formatId);
	static int fileformat2Int(OctMarkerFileformat format);
	
	
	bool saveDefaultMarker();
	bool loadDefaultMarker();
	
	bool loadMarkers(const std::string& markersFilename, OctMarkerFileformat format);
	bool saveMarkers(const std::string& markersFilename, OctMarkerFileformat format);
	
	bool saveMarkersSeries(const std::string& markersFilename);
	bool addMarkersSeries (const std::string& markersFilename);
};

#endif // OCTMARKERIO_H
