#ifndef THICKNESSMAP_H
#define THICKNESSMAP_H


#include "bscanlayersegmentation.h"


namespace cv { class Mat; }

class ThicknessMap
{
	cv::Mat* thicknessMap = nullptr;
public:
	void createMap(const OctData::Series* series, const std::vector<BScanLayerSegmentation::BScanSegData>& lines, OctData::Segmentationlines::SegmentlineType t1, OctData::Segmentationlines::SegmentlineType t2);
};

#endif // THICKNESSMAP_H
