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

#include "bscansegmentationptree.h"

#include <iostream>

#include <vector>

#include <opencv/cv.h>

#include <boost/property_tree/ptree.hpp>
#include <boost/lexical_cast.hpp>
namespace bpt = boost::property_tree;

#include <sstream>

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

#include <boost/serialization/vector.hpp>
#include <boost/serialization/string.hpp>


#include "bscansegmentation.h"

#include <data_structure/simplecvmatcompress.h>



bool BScanSegmentationPtree::parsePTree(const boost::property_tree::ptree& ptree, BScanSegmentation* markerManager)
{
	const char* bscansNodeStr = "ILM";
	boost::optional<const bpt::ptree&> bscansNode = ptree.get_child_optional(bscansNodeStr);

	if(!bscansNode)
		return false;


	for(const std::pair<const std::string, const bpt::ptree>& bscanPair : *bscansNode)
	{
		if(bscanPair.first != "BScan")
			continue;

		try{
			const bpt::ptree& bscanNode = bscanPair.second;
			int bscanId                 = bscanNode.get_child("ID").get_value<int>(-1);
			if(bscanId == -1)
				continue;

			SimpleCvMatCompress* compressedMat = markerManager->segments.at(bscanId);

			if(!compressedMat)
				continue;

			boost::optional<const bpt::ptree&> matCompressNode = bscanNode.get_child_optional("matCompress");
			if(matCompressNode)
			{
				std::string serializationString = matCompressNode->get_value<std::string>("");

				if(serializationString.size() < 2)
					continue;

				std::stringstream ioa(serializationString);
				boost::archive::text_iarchive oa(ioa);
				oa >> *compressedMat;

// 				compressedMat.writeMat(*map);
			}


		}
		catch(...)
		{
			// TODO: Error message
		}
	}
	return true;
}


void BScanSegmentationPtree::fillPTree(boost::property_tree::ptree& markerTree, const BScanSegmentation* markerManager)
{
	markerTree.erase("ILM");
	bpt::ptree& ilmTree = markerTree.put("ILM", std::string());

	std::size_t numBscans = markerManager->getNumBScans();
	for(std::size_t bscan = 0; bscan < numBscans; ++bscan)
	{
		// const cv::Mat* map = markerManager->segments.at(bscan);
		const SimpleCvMatCompress* compressedMat = markerManager->segments.at(bscan);
		if(compressedMat)
		{
			if(compressedMat->isEmpty(BScanSegmentationMarker::paintArea0Value))
				continue;

			std::stringstream ofs;
			boost::archive::text_oarchive oa(ofs);
			// write class instance to archive
			oa << *compressedMat;

			std::string nodeName = "BScan";
			bpt::ptree& bscanNode = ilmTree.add(nodeName, "");
			bscanNode.add("ID", boost::lexical_cast<std::string>(bscan));
			bscanNode.put("matCompress", ofs.str());
		}
	}
}

