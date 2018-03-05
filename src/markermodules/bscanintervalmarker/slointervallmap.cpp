#include "slointervallmap.h"

#include<algorithm>

#include<opencv/cv.hpp>

#include<octdata/datastruct/series.h>
#include<octdata/datastruct/bscan.h>

#include<data_structure/slobscandistancemap.h>


SloIntervallMap::SloIntervallMap()
: sloMap(new cv::Mat)
{
}

SloIntervallMap::~SloIntervallMap()
{
	delete sloMap;
}


void SloIntervallMap::createMap(const SloBScanDistanceMap& distanceMap, const std::vector<BScanIntervalMarker::MarkerMap>& lines, const OctData::Series* series)
{
	const SloBScanDistanceMap::PreCalcDataMatrix* distMatrix = distanceMap.getDataMatrix();

	if(!distMatrix)
		return;

	const std::size_t sizeX = distMatrix->getSizeX();
	const std::size_t sizeY = distMatrix->getSizeY();

	fillCache(lines, series);

	sloMap->create(static_cast<int>(sizeX), static_cast<int>(sizeY), CV_8UC4);

	for(std::size_t y = 0; y < sizeY; ++y)
	{
		uint8_t* destPtr = sloMap->ptr<uint8_t>(static_cast<int>(y));
		const SloBScanDistanceMap::PreCalcDataMatrix::value_type* srcPtr = distMatrix->scanLine(y);

		for(std::size_t x = 0; x < sizeX; ++x)
		{
			if(srcPtr->init)
			{
				const Color& c = getColor(srcPtr->bscan1.bscan, srcPtr->bscan1.ascan);

				destPtr[0] = c.r;
				destPtr[1] = c.g;
				destPtr[2] = c.b;
				destPtr[3] = c.a;
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

const SloIntervallMap::Color& SloIntervallMap::getColor(std::size_t bscan, std::size_t ascan) const
{
	try
	{
		return colorCache.at(bscan).at(ascan);
	}
	catch(std::out_of_range)
	{
		static Color invalidIndex;
		return invalidIndex;
	}
}



void SloIntervallMap::fillCache(const std::vector<BScanIntervalMarker::MarkerMap>& lines, const OctData::Series* series)
{
	if(!series)
		return;

	const std::size_t numBscans = std::min(series->bscanCount(), lines.size());

	colorCache.resize(numBscans);
	for(std::size_t i = 0; i < numBscans; ++i)
	{
		const OctData::BScan* bscan = series->getBScan(i);
		if(!bscan)
			continue;

		BScanIntervalMarker::MarkerMap markerMap = lines[i];
		std::size_t bscanWidth = static_cast<std::size_t>(bscan->getWidth());

		std::vector<Color>& colorLine = colorCache[i];
		colorLine.resize(bscanWidth);

		for(const BScanIntervalMarker::MarkerMap::interval_mapping_type pair : markerMap)
		{
			IntervalMarker::Marker marker = pair.second;
			if(marker.isDefined())
			{
				boost::icl::discrete_interval<int> itv  = pair.first;

				const std::size_t ascanBegin = static_cast<std::size_t>(itv.lower());
				const std::size_t ascanEnd   = std::min(static_cast<std::size_t>(itv.upper()), bscanWidth);

				Color c(marker.getRed(), marker.getGreen(), marker.getBlue(), 255);

				for(std::size_t ascan = ascanBegin; ascan < ascanEnd; ++ascan)
					colorLine[ascan] = c;
			}
		}
	}
}
