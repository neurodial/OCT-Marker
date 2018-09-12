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

#ifndef EXTRASERIESDATA_H
#define EXTRASERIESDATA_H

#include<vector>

#include<boost/property_tree/ptree_fwd.hpp>


#include<data_structure/conturesegment.h>


namespace OctData
{
	class Series;
	class BScan;
}

namespace bpt = boost::property_tree;


class ExtraImageData
{
	std::vector<ContureSegment> contourSegments;
public:
	const std::vector<ContureSegment>& getContourSegments()   const { return contourSegments; }

	void addContureSegment(ContureSegment&& segment)                { contourSegments.push_back(std::move(segment)); }
};



class ExtraSeriesData
{
public:
	typedef std::vector<ExtraImageData> ExtraBScanData;

	bool loadExtraData(const OctData::Series& series, const bpt::ptree& ptree);


	ExtraImageData* getBScanExtraData(std::size_t bscanNum)         { if(bscanNum < extraBScanData.size()) return &extraBScanData[bscanNum]; return nullptr; }

private:
	ExtraBScanData extraBScanData;
};

#endif // EXTRASERIESDATA_H
