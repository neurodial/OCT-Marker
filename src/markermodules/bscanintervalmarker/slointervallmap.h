#ifndef SLOINTERVALLMAP_H
#define SLOINTERVALLMAP_H

#include<vector>

#include "bscanintervalmarker.h"

namespace cv { class Mat; }
namespace OctData { class Series; }

class SloBScanDistanceMap;

class SloIntervallMap
{
public:
	SloIntervallMap();
	~SloIntervallMap();


	SloIntervallMap(const SloIntervallMap& other) = delete;
	SloIntervallMap& operator=(const SloIntervallMap& other) = delete;


	void createMap(const SloBScanDistanceMap& distanceMap
	             , const std::vector<BScanIntervalMarker::MarkerMap>& lines
	             , const OctData::Series* series);

	const cv::Mat& getSloMap() const { return *sloMap; }

private:
	struct Color
	{
		Color() = default;
		Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a) : r(r), g(g), b(b), a(a) {}
		uint8_t r = 0;
		uint8_t g = 0;
		uint8_t b = 0;
		uint8_t a = 0;
	};

	void fillCache(const std::vector<BScanIntervalMarker::MarkerMap>& lines, const OctData::Series* series);
	const Color& getColor(std::size_t bscan, std::size_t ascan) const;

	std::vector<std::vector<Color>> colorCache;
	cv::Mat* sloMap = nullptr;
};

#endif // SLOINTERVALLMAP_H
