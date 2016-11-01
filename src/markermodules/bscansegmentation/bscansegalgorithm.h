#ifndef BSCANSEGALGORITHM_H
#define BSCANSEGALGORITHM_H

#include "configdata.h"

namespace cv
{
	class Mat;
};

class BScanSegmentation;

class BScanSegAlgorithm
{
public:
	static void initFromThreshold(const cv::Mat& image, cv::Mat& segMat, const BScanSegmentationMarker::ThresholdData& data);
};

#endif // BSCANSEGALGORITHM_H
