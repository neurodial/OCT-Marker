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

// need only old format (for mat size)
#include <octdata/datastruct/bscan.h>


namespace
{


	class ColSegments
	{
		friend class boost::serialization::access;

		int colNr = -1;
		std::vector<int> segmentsChange;

		template<class Archive>
		void serialize(Archive & ar, const unsigned int /*version*/)
		{
			ar & colNr;
			ar & segmentsChange;
		}
	public:
		ColSegments(int colNr) : colNr(colNr)                          {}
		ColSegments() = default;
		void addSegmentChange(int rowNr)                               { segmentsChange.push_back(rowNr ); }

		std::size_t size() const                                       { return segmentsChange.size(); }
		bool empty() const                                             { return segmentsChange.empty(); }
		int getColNr() const                                           { return colNr; }

		const std::vector<int>& getInternalVec() const                 { return segmentsChange; }
	};

	class BScanSegments
	{
		friend class boost::serialization::access;

		std::vector<ColSegments> rows;

		template<class Archive>
		void serialize(Archive & ar, const unsigned int /*version*/)
		{
			ar & rows;
		}
	public:
		void addSegment(const ColSegments& row)
		{
			if(!row.empty())
				rows.push_back(row);
		}

		const ColSegments* getSegment(int col)
		{
			for(const ColSegments& colSeg : rows)
				if(colSeg.getColNr() == col)
					return &colSeg;
			return nullptr;
		}

		std::size_t size() const                                       { return rows.size(); }
		bool empty() const                                             { return rows.empty(); }
	};

	void oldDeSerialization(const bpt::ptree& bscanNode, cv::Mat* map)
	{
		std::string serializationString = bscanNode.get_child("serialization").get_value<std::string>("");

		if(serializationString.size() < 2)
			return;

		BScanSegments segments;
		std::stringstream ioa(serializationString);
		boost::archive::text_iarchive oa(ioa);
		oa >> segments;


		BScanSegmentationMarker::internalMatType* colIt = map->ptr<BScanSegmentationMarker::internalMatType>();
		int rowSize = map->rows;
		int colSize = map->cols;

		for(int col = 0; col < colSize; ++col, ++colIt)
		{
			const ColSegments* colSeg = segments.getSegment(col);
			if(!colSeg)
				continue;

			const std::vector<int>& segmentsChange = colSeg->getInternalVec();
			if(segmentsChange.empty())
				continue;

			std::vector<int>::const_iterator it = segmentsChange.begin();
			const std::vector<int>::const_iterator itEnd = segmentsChange.end();

			int segmentChangeCol = *it;
			++it;

			BScanSegmentationMarker::internalMatType value  = BScanSegmentationMarker::markermatInitialValue;
			BScanSegmentationMarker::internalMatType* rowIt = colIt;
			ColSegments colSegments(col);

			for(int row = 0; row < rowSize; ++row)
			{
				if(row == segmentChangeCol)
				{
					if(it != itEnd)
					{
						segmentChangeCol = *it;
						++it;
					}

					if(value == BScanSegmentationMarker::paintArea0Value)
						value = BScanSegmentationMarker::paintArea1Value;
					else
						value = BScanSegmentationMarker::paintArea0Value;
				}
				*rowIt = value;
				rowIt += colSize;
			}
		}
	}


}




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

			// for old format
			boost::optional<const bpt::ptree&> serializationNode = bscanNode.get_child_optional("serialization");
			if(serializationNode)
			{
				const OctData::BScan* bscan = markerManager->getBScan(bscanId);
				if(bscan)
				{
					cv::Mat tempSegMat(bscan->getHeight(), bscan->getWidth(), cv::DataType<uint8_t>::type, cvScalar(BScanSegmentationMarker::markermatInitialValue));
					oldDeSerialization(bscanNode, &tempSegMat);
					compressedMat->readFromMat(tempSegMat);
				}
			}

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

