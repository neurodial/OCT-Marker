#ifndef EXTRASERIESDATA_H
#define EXTRASERIESDATA_H

#include<vector>

#include<boost/property_tree/ptree_fwd.hpp>


#include<data_structure/conturesegment.h>


namespace OctData
{
	class Series;
	class BScan;
}

namespace bpt = boost::property_tree;


class ExtraImageData
{
	std::vector<ContureSegment> contourSegments;
public:
	const std::vector<ContureSegment>& getContourSegments()   const { return contourSegments; }

	void addContureSegment(ContureSegment&& segment)                { contourSegments.push_back(std::move(segment)); }
};



class ExtraSeriesData
{
	std::vector<ExtraImageData> extraBScanData;
public:
	bool loadExtraData(const OctData::Series& series, const bpt::ptree& ptree);


	ExtraImageData* getBScanExtraData(std::size_t bscanNum)         { if(bscanNum < extraBScanData.size()) return &extraBScanData[bscanNum]; return nullptr; }
};

#endif // EXTRASERIESDATA_H
