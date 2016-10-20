#ifndef OCTMARKERDATA_H
#define OCTMARKERDATA_H

#include <string>

#include <boost/property_tree/ptree_fwd.hpp>


class OctMarkerBScanData
{
	boost::property_tree::ptree& ptree;
	
public:
	OctMarkerBScanData(boost::property_tree::ptree& ptree) : ptree(ptree) {}
};


class OctMarkerData
{
	boost::property_tree::ptree* ptree;
	
public:
	OctMarkerData();
	~OctMarkerData();
	
	OctMarkerBScanData getOctMarkerBScanData(int patient, int study, int series);
	
	enum class Fileformat { XML, Josn };
	
	bool loadData(const std::string& filename);
	bool saveData(const std::string& filename);
	
};

#endif // OCTMARKERDATA_H
