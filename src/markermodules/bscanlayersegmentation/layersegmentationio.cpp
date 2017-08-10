#include "layersegmentationio.h"

#include"bscanlayersegmentation.h"

#include <cpp_framework/cvmat/cvmattreestruct.h>
#include <cpp_framework/cvmat/treestructbin.h>

#include<opencv/cv.h>


bool LayerSegmentationIO::saveSegmentation2Bin(const BScanLayerSegmentation& marker, const std::string& filename)
{

	const std::vector<BScanLayerSegmentation::BScanSegData>& lines = marker.lines;

	const int numBscans     = static_cast<int>(lines.size()             );
	const int maxBscanWidth = static_cast<int>(marker.getMaxBscanWidth());

	std::map<const char*, cv::Mat> segmentationMats;


	// create matrizen
	for(OctData::Segmentationlines::SegmentlineType type : OctData::Segmentationlines::getSegmentlineTypes())
	{
		const char* name = OctData::Segmentationlines::getSegmentlineName(type);
		segmentationMats[name].create(numBscans, maxBscanWidth, cv::DataType<float>::type);
	}

	// fill matrizen
	int bscan = 0;
	for(const BScanLayerSegmentation::BScanSegData& bscanData : lines)
	{
		const OctData::Segmentationlines& lines = bscanData.lines;

		for(OctData::Segmentationlines::SegmentlineType type : OctData::Segmentationlines::getSegmentlineTypes())
		{
			const char* name = OctData::Segmentationlines::getSegmentlineName(type);
			const OctData::Segmentationlines::Segmentline& line = lines.getSegmentLine(type);
			float* ptr = segmentationMats[name].ptr<float>(bscan);

			std::copy(line.begin(), line.end(), ptr);
		}

		++bscan;
	}

	// create matrizen
	CppFW::CVMatTree tree;
	for(OctData::Segmentationlines::SegmentlineType type : OctData::Segmentationlines::getSegmentlineTypes())
	{
		const char* name = OctData::Segmentationlines::getSegmentlineName(type);
		tree.getDirNode(name).getMat() = segmentationMats[name];
	}

	CppFW::CVMatTreeStructBin::writeBin(filename, tree);
	return true;
}
