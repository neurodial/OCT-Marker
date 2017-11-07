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

