#include "importintervalmarker.h"

#include <algorithm>

#include <cpp_framework/cvmat/cvmattreestruct.h>
#include <cpp_framework/cvmat/treestructbin.h>

#include <data_structure/intervalmarker.h>

#include "bscanintervalmarker.h"

#include <opencv/cv.h>


namespace
{
	IntervalMarker::Marker getMarker(int nr, std::vector<IntervalMarker::Marker>& mapping)
	{
		if(nr < 0 || static_cast<std::size_t>(nr) >= mapping.size())
			return IntervalMarker::Marker();
		return mapping[static_cast<std::size_t>(nr)];
	}
}


bool ImportIntervalMarker::importBin(BScanIntervalMarker* markerManager, const IntervalMarker& markerCollection, const std::string& filename)
{
	CppFW::CVMatTree tree = CppFW::CVMatTreeStructBin::readBin(filename);

	const CppFW::CVMatTree* marker = tree.getDirNodeOpt("marker");
	const CppFW::CVMatTree* field  = tree.getDirNodeOpt("field");

	if(!marker || !field)
		return false;

	if(marker->type() != CppFW::CVMatTree::Type::List
	|| field ->type() != CppFW::CVMatTree::Type::Mat )
		return false;

	const CppFW::CVMatTree::NodeList& markerNodeList = marker->getNodeList();
	cv::Mat                           mat            = field->getMat();
	if(mat.type() != cv::DataType<uint8_t>::type)
		return false;

	std::vector<IntervalMarker::Marker> markerMap(markerNodeList.size());

	auto itMarkerMap = markerMap.begin();
	for(const CppFW::CVMatTree* markerNode : markerNodeList)
	{
		if(markerNode->type() == CppFW::CVMatTree::Type::String)
		{
			try
			{
				IntervalMarker::Marker marker = markerCollection.getMarkerFromString(markerNode->getString());
				*itMarkerMap = marker;
			}
			catch(const std::out_of_range& e)
			{
				std::cerr << e.what() << '\n';
			}
		}
		++itMarkerMap;
	}

	// TODO: bscan & ascan größe prüfen

	std::size_t numBScans = markerManager->getNumBScans();
	if(static_cast<std::size_t>(mat.rows) != numBScans && static_cast<std::size_t>(mat.cols) == numBScans)
		mat = mat.t();

	std::size_t importBScans = std::min(numBScans, static_cast<std::size_t>(mat.rows));

	for(std::size_t row = 0; row < importBScans; ++row)
	{
		const uint8_t* markerFieldIt = mat.ptr<uint8_t>(static_cast<int>(row));

		int lastMarkerPos = 0;
		IntervalMarker::Marker lastMarker = getMarker(markerFieldIt[0], markerMap);
		for(int col = 1; col < mat.cols; ++col)
		{
			const IntervalMarker::Marker actMarker = getMarker(markerFieldIt[col], markerMap);
			if(actMarker != lastMarker)
			{
				markerManager->setMarker(lastMarkerPos, col, lastMarker, row);
				lastMarkerPos = col;
				lastMarker = actMarker;
			}
		}
		markerManager->setMarker(lastMarkerPos, mat.cols, lastMarker, row);
	}

	return true;
}
