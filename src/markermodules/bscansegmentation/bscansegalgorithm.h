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

#ifndef BSCANSEGALGORITHM_H
#define BSCANSEGALGORITHM_H

#include "configdata.h"

#include <octdata/datastruct/segmentationlines.h>

namespace cv
{
	class Mat;
}

namespace OctData
{
	class BScan;
}

class BScanSegmentation;

class BScanSegAlgorithm
{
public:
	typedef BScanSegmentationMarker::internalMatType PaintType;

	static void initFromSegline(const OctData::BScan& bscan, cv::Mat& segMat, OctData::Segmentationlines::SegmentlineType type);
	static void initFromThresholdDirection(const cv::Mat& image, cv::Mat& segMat, const BScanSegmentationMarker::ThresholdDirectionData& data, PaintType val0, PaintType val1);
	static PaintType getThresholdGrayValue(const cv::Mat& image, const BScanSegmentationMarker::ThresholdData& data);
	static void initFromThreshold(const cv::Mat& image, cv::Mat& segMat, const BScanSegmentationMarker::ThresholdData& data, PaintType val0, PaintType val1);
	static void openClose(cv::Mat& dest, cv::Mat* src = nullptr); /// if no src given, then dest is used as src
	static bool removeUnconectedAreas(cv::Mat& image);
	static bool extendLeftRightSpace(cv::Mat& image, int limit = 40);
};

#endif // BSCANSEGALGORITHM_H
