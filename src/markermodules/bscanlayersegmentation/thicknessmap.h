#ifndef THICKNESSMAP_H
#define THICKNESSMAP_H

#include<vector>

#include "bscanlayersegmentation.h"

#include<data_structure/matrx.h>
#include<data_structure/slobscandistancemap.h>
#include<limits>

#include<octdata/datastruct/segmentationlines.h>

class Colormap;
namespace cv { class Mat; }

class ThicknessMap
{
public:

	ThicknessMap();
	~ThicknessMap();

	ThicknessMap(const ThicknessMap& other) = delete;
	ThicknessMap& operator=(const ThicknessMap& other) = delete;

	void resetThicknessMapCache();


	void createMap(const SloBScanDistanceMap& distanceMap
	             , const std::vector<BScanLayerSegmentation::BScanSegData>& lines
	             , OctData::Segmentationlines::SegmentlineType t1
	             , OctData::Segmentationlines::SegmentlineType t2
	             , double scaleFactor
                 , const Colormap& colormap);

	const cv::Mat& getThicknessMap() const { return *thicknessMap; }

private:
	cv::Mat* thicknessMap = nullptr;

	double getMixValue(const SloBScanDistanceMap::PixelInfo& pinfo);
	double getHeight(const OctData::Segmentationlines::Segmentline& line, const std::size_t ascan);
	double getValue(const SloBScanDistanceMap::InfoBScanDist& info);

	void fillLineVec(const std::vector<BScanLayerSegmentation::BScanSegData>& lines
	               , OctData::Segmentationlines::SegmentlineType t1
	               , OctData::Segmentationlines::SegmentlineType t2);

	std::vector<const OctData::Segmentationlines::Segmentline*> layer1;
	std::vector<const OctData::Segmentationlines::Segmentline*> layer2;
};

#endif // THICKNESSMAP_H
