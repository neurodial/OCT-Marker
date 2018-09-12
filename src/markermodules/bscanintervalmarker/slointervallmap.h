/*
 * Copyright (c) 2018 Kay Gawlik <kaydev@amarunet.de> <kay.gawlik@beuth-hochschule.de> <kay.gawlik@charite.de>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

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
