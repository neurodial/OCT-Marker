#include "thicknessmap.h"


#define _USE_MATH_DEFINES

#include<map>
#include<limits>
#include<cmath>

#include<opencv/cv.hpp>

#include<octdata/datastruct/series.h>
#include<octdata/datastruct/bscan.h>
#include<octdata/datastruct/sloimage.h>

#include<helper/convertcolorspace.h>

#include<data_structure/matrx.h>
#include<data_structure/point2d.h>
#include<data_structure/programoptions.h>

#include"colormaphsv.h"


using Segmentline         = OctData::Segmentationlines::Segmentline;
using SegmentlineDataType = OctData::Segmentationlines::SegmentlineDataType;



ThicknessMap::ThicknessMap()
: thicknessMap(new cv::Mat)
{
}



ThicknessMap::~ThicknessMap()
{
	delete thicknessMap;
}


namespace
{
	double getHeight(const BScanLayerSegmentation::BScanSegData& lines, const OctData::Segmentationlines::SegmentlineType t1, const std::size_t ascan)
	{
		if(lines.filled)
		{
			const OctData::Segmentationlines::Segmentline line = lines.lines.getSegmentLine(t1);
			if(line.size() > ascan)
				return line[ascan];
		}
		return std::numeric_limits<double>::quiet_NaN();
	}

	double getValue(const std::vector<BScanLayerSegmentation::BScanSegData>& lines, const SloBScanDistanceMap::InfoBScanDist& info
                           , OctData::Segmentationlines::SegmentlineType t1
                           , OctData::Segmentationlines::SegmentlineType t2)
	{
		std::size_t ascan = info.ascan;
		std::size_t bscan = info.bscan;

		if(bscan >= lines.size())
			return 0.;

		const BScanLayerSegmentation::BScanSegData& segData = lines[bscan];

		double h1 = getHeight(segData, t1, ascan);
		double h2 = getHeight(segData, t2, ascan);

		return h1-h2;
	}

	double getMixValue(const std::vector<BScanLayerSegmentation::BScanSegData>& lines, const SloBScanDistanceMap::PixelInfo& pinfo
                           , OctData::Segmentationlines::SegmentlineType t1
                           , OctData::Segmentationlines::SegmentlineType t2)
	{
		const SloBScanDistanceMap::InfoBScanDist& bInfo1 = pinfo.bscan1;
		const SloBScanDistanceMap::InfoBScanDist& bInfo2 = pinfo.bscan2;


		std::size_t b1 = bInfo1.bscan;
		std::size_t b2 = bInfo2.bscan;

		const std::size_t numBscans = lines.size();

		if(b1 >= numBscans)
			return 0;

		double h1 = getValue(lines, bInfo1, t1, t2);

		if(h1 <= 0)
			return 0;
		if(b2 >= numBscans)
			return h1;

		double h2 = getValue(lines, bInfo2, t1, t2);
		if(h2 <= 0)
			return h1;

		const double l = bInfo1.distance + bInfo2.distance;
		if(l == 0)
			return h1;

		const double w1 = bInfo2.distance/l;
		const double w2 = bInfo1.distance/l;

		const double result = h1*w1 + h2*w2;

		return result;
	}
}


void ThicknessMap::createMap(const SloBScanDistanceMap& distMap
                           , const std::vector<BScanLayerSegmentation::BScanSegData>& lines
                           , OctData::Segmentationlines::SegmentlineType t1
                           , OctData::Segmentationlines::SegmentlineType t2
                           , double scaleFactor
                           , const Colormap& colormap)
{
	SloBScanDistanceMap::PreCalcDataMatrix* distMatrix = distMap.getDataMatrix();

	if(!distMatrix)
		return;

	const std::size_t sizeX = distMatrix->getSizeX();
	const std::size_t sizeY = distMatrix->getSizeY();


	thicknessMap->create(static_cast<int>(sizeX), static_cast<int>(sizeY), CV_8UC4);

	for(std::size_t y = 0; y < sizeX; ++y)
	{
		uint8_t* destPtr = thicknessMap->ptr<uint8_t>(static_cast<int>(y));
		SloBScanDistanceMap::PreCalcDataMatrix::value_type* srcPtr = distMatrix->scanLine(y);

		for(std::size_t x = 0; x < sizeX; ++x)
		{
			if(srcPtr->init)
			{
				double value = getMixValue(lines, *srcPtr, t1, t2);
				double mixThickness = value*scaleFactor;

				colormap.getColor(mixThickness, destPtr[0], destPtr[1], destPtr[2]);
				destPtr[3] = 255;

			}
			else
			{
				destPtr[0] = 0;
				destPtr[1] = 0;
				destPtr[2] = 0;
				destPtr[3] = 0;
			}

			destPtr += 4;
			++srcPtr;
		}

	}

// 	CreateThicknessMap thicknessMapCreator(series, lines, t1, t2);
//
// 	thicknessMapCreator.setBlendColor(ProgramOptions::layerSegThicknessmapBlend());
// 	thicknessMapCreator.createMap();
// 	thicknessMapCreator.getThicknessMap().copyTo(*thicknessMap);
}

