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

	fillLineVec(lines, t1, t2);

	thicknessMap->create(static_cast<int>(sizeX), static_cast<int>(sizeY), CV_8UC4);

	for(std::size_t y = 0; y < sizeX; ++y)
	{
		uint8_t* destPtr = thicknessMap->ptr<uint8_t>(static_cast<int>(y));
		SloBScanDistanceMap::PreCalcDataMatrix::value_type* srcPtr = distMatrix->scanLine(y);

		for(std::size_t x = 0; x < sizeX; ++x)
		{
			if(srcPtr->init)
			{
// 				double value = getMixValue(lines, *srcPtr, t1, t2);
				double value = getMixValue(*srcPtr);
				if(value <= 0.)
				{
					destPtr[0] = 0;
					destPtr[1] = 0;
					destPtr[2] = 0;
					destPtr[3] = 0;
				}
				else
				{
					double mixThickness = value*scaleFactor;

					colormap.getColor(mixThickness, destPtr[0], destPtr[1], destPtr[2]);
					destPtr[3] = 255;
				}

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

double ThicknessMap::getMixValue(const SloBScanDistanceMap::PixelInfo& pinfo)
{
	const SloBScanDistanceMap::InfoBScanDist& bInfo1 = pinfo.bscan1;
	const SloBScanDistanceMap::InfoBScanDist& bInfo2 = pinfo.bscan2;


	std::size_t b1 = bInfo1.bscan;
	std::size_t b2 = bInfo2.bscan;

	const std::size_t numBscans = layer1.size();

	if(b1 >= numBscans)
		return 0;

	double h1 = getValue( bInfo1);

	if(h1 <= 0)
		return 0;
	if(b2 >= numBscans)
		return h1;

	double h2 = getValue(bInfo2);
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

double ThicknessMap::getHeight(const OctData::Segmentationlines::Segmentline& line, const std::size_t ascan)
{
	if(line.size() > ascan)
	{
		double val = line[ascan];
		if(val < 1e6)
			return val;
	}
	return -1;
}

double ThicknessMap::getValue(const SloBScanDistanceMap::InfoBScanDist& info)
{
	std::size_t ascan = info.ascan;
	std::size_t bscan = info.bscan;

	if(bscan >= layer1.size() && bscan >= layer2.size())
		return 0.;

	const OctData::Segmentationlines::Segmentline* line1 = layer1[bscan];
	const OctData::Segmentationlines::Segmentline* line2 = layer2[bscan];


	if(!line1 || !line2)
		return 0;

	double h1 = getHeight(*line1, ascan);
	double h2 = getHeight(*line2, ascan);

	if(h1 >= 0 && h2 >= 0)
		return h2-h1;
	return 0;
}



void ThicknessMap::fillLineVec(const std::vector<BScanLayerSegmentation::BScanSegData>& lines
                             , OctData::Segmentationlines::SegmentlineType t1
                             , OctData::Segmentationlines::SegmentlineType t2)
{
	layer1.clear();
	layer2.clear();

	for(const BScanLayerSegmentation::BScanSegData& segData : lines)
	{
		const OctData::Segmentationlines::Segmentline* l1 = nullptr;
		const OctData::Segmentationlines::Segmentline* l2 = nullptr;
		if(segData.filled)
		{
			l1 = &(segData.lines.getSegmentLine(t1));
			l2 = &(segData.lines.getSegmentLine(t2));
		}

		layer1.push_back(l1);
		layer2.push_back(l2);
	}
}

void ThicknessMap::resetThicknessMapCache()
{
}
