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

		const bpt::ptree& bscanNode = bscanPair.second;
		int bscanId             = bscanNode.get_child("ID").get_value<int>(-1);
		if(bscanId == -1)
			continue;

		cv::Mat* map = markerManager->segments.at(bscanId);

		if(!map || map->empty())
			continue;

		std::string serializationString = bscanNode.get_child("serialization").get_value<std::string>("");

		if(serializationString.size() < 2)
			continue;

		BScanSegments segments;
		std::stringstream ioa(serializationString);
		boost::archive::text_iarchive oa(ioa);
		oa >> segments;


		BScanSegmentation::internalMatType* colIt = map->ptr<BScanSegmentation::internalMatType>();
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

			BScanSegmentation::internalMatType value  = BScanSegmentation::initialValue;
			BScanSegmentation::internalMatType* rowIt = colIt;
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

					if(value == BScanSegmentation::paintArea0Value)
						value = BScanSegmentation::paintArea1Value;
					else
						value = BScanSegmentation::paintArea0Value;
				}
				*rowIt = value;
				rowIt += colSize;
			}
		}
	}
	return true;
}


void BScanSegmentationPtree::fillPTree(boost::property_tree::ptree& markerTree, const BScanSegmentation* markerManager)
{
	markerTree.erase("ILM");
	bpt::ptree& ilmTree = markerTree.put("ILM", std::string());

	int numBscans = markerManager->getNumBScans();
	for(int bscan = 0; bscan < numBscans; ++bscan)
	{
		BScanSegments segments;

		cv::Mat* map = markerManager->segments.at(bscan);

		if(map && !map->empty())
		{
			BScanSegmentation::internalMatType* colIt = map->ptr<BScanSegmentation::internalMatType>();
			int rowSize = map->rows;
			int colSize = map->cols;

			for(int col = 0; col < colSize; ++col)
			{
				BScanSegmentation::internalMatType value  = BScanSegmentation::initialValue;
				BScanSegmentation::internalMatType* rowIt = colIt;
				ColSegments colSegments(col);

				for(int row = 0; row < rowSize; ++row)
				{
					if(value != *rowIt)
					{
						value = *rowIt;
						colSegments.addSegmentChange(row);
					}
					rowIt += colSize;
				}

				++colIt;
				segments.addSegment(colSegments);
			}


			if(segments.empty())
				continue;


			std::stringstream ofs;
			boost::archive::text_oarchive oa(ofs);
			// write class instance to archive
			oa << segments;

			std::string nodeName = "BScan";
			bpt::ptree& bscanNode = ilmTree.add(nodeName, "");
			bscanNode.add("ID", boost::lexical_cast<std::string>(bscan));
			bscanNode.put("serialization", ofs.str());
		}
	}

}

/*
if(mat && !mat->empty())
{
	internalMatType* colIt = mat->ptr<internalMatType>();
	std::size_t colSize = static_cast<std::size_t>(mat->cols);
	std::size_t rowSize = static_cast<std::size_t>(mat->rows);
	for(double value : segline)
	{
		const std::size_t rowCh = std::min(static_cast<std::size_t>(value), rowSize);
		internalMatType* rowIt = colIt;

		for(std::size_t row = 0; row < rowCh; ++row)
		{
			*rowIt =  1;
			rowIt += colSize;
		}

		for(std::size_t row = rowCh; row < rowSize; ++row)
		{
			*rowIt = 0;
			rowIt += colSize;
		}

		++colIt;
	}
}
*/
