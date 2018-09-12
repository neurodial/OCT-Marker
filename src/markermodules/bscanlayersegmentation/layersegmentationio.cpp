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

#include "layersegmentationio.h"

#include"bscanlayersegmentation.h"

#include <oct_cpp_framework/cvmat/cvmattreestruct.h>
#include <oct_cpp_framework/cvmat/treestructbin.h>

#include<opencv/cv.h>


bool LayerSegmentationIO::saveSegmentation2Bin(const BScanLayerSegmentation& marker, const std::string& filename)
{

	const std::vector<BScanLayerSegmentation::BScanSegData>& lines = marker.lines;

	const int numBscans     = static_cast<int>(lines.size()             );
	const int maxBscanWidth = static_cast<int>(marker.getMaxBscanWidth());

	std::map<const char*, cv::Mat> segmentationMats;


	// create matrizen
	for(OctData::Segmentationlines::SegmentlineType type : OctData::Segmentationlines::getSegmentlineTypes())
	{
		const char* name = OctData::Segmentationlines::getSegmentlineName(type);
		segmentationMats[name].create(numBscans, maxBscanWidth, cv::DataType<float>::type);
	}

	// fill matrizen
	int bscan = 0;
	for(const BScanLayerSegmentation::BScanSegData& bscanData : lines)
	{
		const OctData::Segmentationlines& lines = bscanData.lines;

		for(OctData::Segmentationlines::SegmentlineType type : OctData::Segmentationlines::getSegmentlineTypes())
		{
			const char* name = OctData::Segmentationlines::getSegmentlineName(type);
			const OctData::Segmentationlines::Segmentline& line = lines.getSegmentLine(type);
			float* ptr = segmentationMats[name].ptr<float>(bscan);

			std::copy(line.begin(), line.end(), ptr);
		}

		++bscan;
	}

	// create matrizen
	CppFW::CVMatTree tree;
	for(OctData::Segmentationlines::SegmentlineType type : OctData::Segmentationlines::getSegmentlineTypes())
	{
		const char* name = OctData::Segmentationlines::getSegmentlineName(type);
		tree.getDirNode(name).getMat() = segmentationMats[name];
	}

	CppFW::CVMatTreeStructBin::writeBin(filename, tree);
	return true;
}
