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

	bool blendColor = ProgramOptions::layerSegThicknessmapBlend();

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
				double value;
				if(blendColor) value = getMixValue(*srcPtr);
				else           value = getSingleValue(*srcPtr);

				if(value < 0.)
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
}

double ThicknessMap::getSingleValue(const SloBScanDistanceMap::PixelInfo& pinfo) const
{
	const SloBScanDistanceMap::InfoBScanDist& bInfo1 = pinfo.bscan1;
	return getValue(bInfo1);
}


double ThicknessMap::getMixValue(const SloBScanDistanceMap::PixelInfo& pinfo) const
{
	const SloBScanDistanceMap::InfoBScanDist& bInfo1 = pinfo.bscan1;
	const SloBScanDistanceMap::InfoBScanDist& bInfo2 = pinfo.bscan2;


	const std::size_t b1 = bInfo1.bscan;
	const std::size_t b2 = bInfo2.bscan;

	const size_t numBscans = thicknessMatrix.getSizeY();

	if(b1 >= numBscans)
		return -1;

	double h1 = getValue(bInfo1);

	if(std::isnan(h1) || h1 < 0)
		return -1;
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


double ThicknessMap::getValue(const SloBScanDistanceMap::InfoBScanDist& info) const
{
	std::size_t ascan = info.ascan;
	std::size_t bscan = info.bscan;

	if(ascan >= thicknessMatrix.getSizeX() || bscan >= thicknessMatrix.getSizeY())
		return std::numeric_limits<double>::quiet_NaN();

	return thicknessMatrix(ascan, bscan);
}



void ThicknessMap::initThicknessMatrix(const std::vector<BScanLayerSegmentation::BScanSegData>& lines, OctData::Segmentationlines::SegmentlineType t1, OctData::Segmentationlines::SegmentlineType t2)
{
	const std::size_t numBscans = lines.size();
	std::size_t maxAscanNum = 0;

	for(const BScanLayerSegmentation::BScanSegData& segData : lines)
	{
		const OctData::Segmentationlines::Segmentline* l1 = nullptr;
		const OctData::Segmentationlines::Segmentline* l2 = nullptr;
		if(segData.filled)
		{
			l1 = &(segData.lines.getSegmentLine(t1));
			l2 = &(segData.lines.getSegmentLine(t2));

			const std::size_t numAscans = std::min(l1->size(), l2->size());
			if(numAscans > maxAscanNum)
				maxAscanNum = numAscans;
		}
	}

	thicknessMatrix.resize(maxAscanNum, numBscans);
}

void ThicknessMap::fillLineVec(const std::vector<BScanLayerSegmentation::BScanSegData>& lines
                             , OctData::Segmentationlines::SegmentlineType t1
                             , OctData::Segmentationlines::SegmentlineType t2)
{
	initThicknessMatrix(lines, t1, t2);
	std::size_t nrBscan = 0;
	for(const BScanLayerSegmentation::BScanSegData& segData : lines)
	{
		fillThicknessBscan(segData, nrBscan, t1, t2);
		++nrBscan;
	}
}

namespace
{
	double getValue(const double* const line, std::size_t index)
	{
		const double value = line[index];
		if(value > 1e8)
			return std::numeric_limits<double>::quiet_NaN();
		return value;
	}
}

void ThicknessMap::fillThicknessBscan(const BScanLayerSegmentation::BScanSegData& bscanData, const std::size_t bscanNr, OctData::Segmentationlines::SegmentlineType t1, OctData::Segmentationlines::SegmentlineType t2)
{
	double* const scanline = thicknessMatrix.scanLine(bscanNr);

	std::size_t filledAscans = 0;
	if(bscanData.filled)
	{
		const OctData::Segmentationlines::Segmentline* l1 = &(bscanData.lines.getSegmentLine(t1));
		const OctData::Segmentationlines::Segmentline* l2 = &(bscanData.lines.getSegmentLine(t2));
		if(l1 && l2)
		{
			const std::size_t numAscans = std::min(l1->size(), l2->size());

			const double* const l1data = l1->data();
			const double* const l2data = l2->data();

			for(std::size_t i = 0; i < numAscans; ++i)
				scanline[i] = ::getValue(l2data, i) - ::getValue(l1data, i);
			filledAscans = numAscans;
		}
	}
	const std::size_t maxAscanNum = thicknessMatrix.getSizeX();
	for(std::size_t i = filledAscans; i < maxAscanNum; ++i)
		scanline[i] = std::numeric_limits<double>::quiet_NaN();
}


void ThicknessMap::resetThicknessMapCache()
{
}
