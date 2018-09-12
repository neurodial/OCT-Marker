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

	double getSingleValue(const SloBScanDistanceMap::PixelInfo& pinfo) const;
	double getMixValue(const SloBScanDistanceMap::PixelInfo& pinfo) const;
	double getValue(const SloBScanDistanceMap::InfoBScanDist& info) const;

	void fillLineVec(const std::vector<BScanLayerSegmentation::BScanSegData>& lines
	               , OctData::Segmentationlines::SegmentlineType t1
	               , OctData::Segmentationlines::SegmentlineType t2);

	void fillThicknessBscan(const BScanLayerSegmentation::BScanSegData& bscan
	                      , const std::size_t bscanNr
	                      , OctData::Segmentationlines::SegmentlineType t1
	                      , OctData::Segmentationlines::SegmentlineType t2);

	void initThicknessMatrix(const std::vector<BScanLayerSegmentation::BScanSegData>& lines
	                       , OctData::Segmentationlines::SegmentlineType t1
	                       , OctData::Segmentationlines::SegmentlineType t2);

	Matrix<double> thicknessMatrix;
};

#endif // THICKNESSMAP_H
