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

#include "importintervalmarker.h"

#include <algorithm>

#include <oct_cpp_framework/cvmat/cvmattreestruct.h>
#include <oct_cpp_framework/cvmat/treestructbin.h>

#include <data_structure/intervalmarker.h>

#include "bscanintervalmarker.h"

#include <opencv/cv.h>
#include "definedintervalmarker.h"
#include <octdata/datastruct/bscan.h>


namespace
{
	IntervalMarker::Marker getMarker(int nr, std::vector<IntervalMarker::Marker>& mapping)
	{
		if(nr < 0 || static_cast<std::size_t>(nr) >= mapping.size())
			return IntervalMarker::Marker();
		return mapping[static_cast<std::size_t>(nr)];
	}


	bool parseMarkerCollection(const CppFW::CVMatTree& node, BScanIntervalMarker* markerManager, BScanIntervalMarker::MarkerCollectionWork& collectionSetterHelper)
	{
		const CppFW::CVMatTree* marker = node.getDirNodeOpt("marker");
		const CppFW::CVMatTree* field  = node.getDirNodeOpt("field");

		if(!marker || !field)
			return false;

		if(marker->type() != CppFW::CVMatTree::Type::List
		|| field ->type() != CppFW::CVMatTree::Type::Mat )
			return false;


		const CppFW::CVMatTree::NodeList& markerNodeList = marker->getNodeList();
		cv::Mat                           mat            = field->getMat();
		if(mat.type() != cv::DataType<uint8_t>::type)
			return false;

		const IntervalMarker* markerCollection = markerManager->getMarkersList(collectionSetterHelper);

		std::vector<IntervalMarker::Marker> markerMap(markerNodeList.size());

		// create marker list
		auto itMarkerMap = markerMap.begin();
		for(const CppFW::CVMatTree* markerNode : markerNodeList)
		{
			if(markerNode->type() == CppFW::CVMatTree::Type::String)
			{
				try
				{
					IntervalMarker::Marker marker = markerCollection->getMarkerFromString(markerNode->getString());
					*itMarkerMap = marker;
				}
				catch(const std::out_of_range& e)
				{
					std::cerr << e.what() << '\n';
				}
			}
			++itMarkerMap;
		}

		// TODO: bscan & ascan größe prüfen

		std::size_t numBScans = markerManager->getNumBScans();
		if(static_cast<std::size_t>(mat.rows) != numBScans && static_cast<std::size_t>(mat.cols) == numBScans)
			mat = mat.t();

		std::size_t importBScans = std::min(numBScans, static_cast<std::size_t>(mat.rows));

		for(std::size_t row = 0; row < importBScans; ++row)
		{
			const uint8_t* markerFieldIt = mat.ptr<uint8_t>(static_cast<int>(row));

			int lastMarkerPos = 0;
			IntervalMarker::Marker lastMarker = getMarker(markerFieldIt[0], markerMap);
			for(int col = 1; col < mat.cols; ++col)
			{
				const IntervalMarker::Marker actMarker = getMarker(markerFieldIt[col], markerMap);
				if(actMarker != lastMarker)
				{
					markerManager->setMarker(lastMarkerPos, col, lastMarker, row, collectionSetterHelper);
					lastMarkerPos = col;
					lastMarker = actMarker;
				}
			}
			markerManager->setMarker(lastMarkerPos, mat.cols, lastMarker, row, collectionSetterHelper);
		}

		return true;
	}
}


bool ImportIntervalMarker::importBin(BScanIntervalMarker* markerManager, const std::string& filename)
{
	CppFW::CVMatTree tree = CppFW::CVMatTreeStructBin::readBin(filename);



	if(tree.type() == CppFW::CVMatTree::Type::Dir)
	{
		const CppFW::CVMatTree* markerMapsNode = tree.getDirNodeOpt("marker_maps");
		if(markerMapsNode)
		{
			bool result = true;

			if(markerMapsNode->type() == CppFW::CVMatTree::Type::List) // old format
			{
				for(const CppFW::CVMatTree* node : markerMapsNode->getNodeList())
				{
					if(!node)
						continue;
					const CppFW::CVMatTree* collectionNameNode = node->getDirNodeOpt("marker_collection");
					if(collectionNameNode && collectionNameNode->type() == CppFW::CVMatTree::Type::String)
					{
						const std::string& collectionName = collectionNameNode->getString();
						std::cerr << "marker_collection: " << collectionName << "\n";
						BScanIntervalMarker::MarkerCollectionWork collectionSetterHelper = markerManager->getMarkerCollection(collectionName);
						result &= parseMarkerCollection(*node, markerManager, collectionSetterHelper);
					}
					else
						result = false;
				}
				return result;
			}
			else if(markerMapsNode->type() == CppFW::CVMatTree::Type::Dir) // new Format
			{
				for(const CppFW::CVMatTree::NodeDir::value_type& node : markerMapsNode->getNodeDir())
				{
					if(!node.second)
						continue;
					const std::string& collectionName = node.first;
					std::cerr << "marker_collection: " << collectionName << "\n";
					BScanIntervalMarker::MarkerCollectionWork collectionSetterHelper = markerManager->getMarkerCollection(collectionName);
					result &= parseMarkerCollection(*node.second, markerManager, collectionSetterHelper);
				}
				return result;
			}

		}
		else // import old data structure
		{
			BScanIntervalMarker::MarkerCollectionWork collectionSetterHelper = markerManager->getActMarkerCollection();
			return parseMarkerCollection(tree, markerManager, collectionSetterHelper);
		}
	}


	std::cerr << "Wrong import format\n";
	return false;
}




bool ImportIntervalMarker::exportBin(BScanIntervalMarker* markerManager, const std::string& filename)
{
	const std::size_t numBscans     =                  markerManager->getNumBScans();
	const int         maxBscanWidth = static_cast<int>(markerManager->getMaxBscanWidth());


	CppFW::CVMatTree tree;

	CppFW::CVMatTree& markerMaps = tree.getDirNode("marker_maps");

	const DefinedIntervalMarker::IntervallMarkerMap& definedIntervalMarker = DefinedIntervalMarker::getInstance().getIntervallMarkerMap();
	for(auto& obj : definedIntervalMarker)
	{
		const std::string& markerCollectionInternalName = obj.first;

		CppFW::CVMatTree& collectionNode = markerMaps.getDirNode(markerCollectionInternalName);

		CppFW::CVMatTree& fieldNode = collectionNode.getDirNode("field");
		CppFW::CVMatTree& markerNode = collectionNode.getDirNode("marker");

		const IntervalMarker& markers = obj.second;
		const IntervalMarker::IntervalMarkerList& intervalMarkerList = markers.getIntervalMarkerList();
		for(const IntervalMarker::Marker& marker : intervalMarkerList)
			markerNode.newListNode().getString() = marker.getInternalName();


		cv::Mat& fieldMat = fieldNode.getMat();
		fieldMat.create(static_cast<int>(numBscans), maxBscanWidth, cv::DataType<uint8_t>::type);
		fieldMat = cv::Scalar(0); // init mat

		for(std::size_t bscan = 0; bscan < numBscans; ++bscan)
		{
			const BScanIntervalMarker::MarkerMap& markerMap = markerManager->getMarkers(markerCollectionInternalName, bscan);
			for(const BScanIntervalMarker::MarkerMap::interval_mapping_type pair : markerMap)
			{

				// std::cout << "paintEvent(QPaintEvent* event) " << pair.second << " - " << pair.first << std::endl;

				IntervalMarker::Marker marker = pair.second;
				if(marker.isDefined())
				{
					boost::icl::discrete_interval<int> itv  = pair.first;

					auto pos = std::find(intervalMarkerList.begin(), intervalMarkerList.end(), marker) - intervalMarkerList.begin();
					int minPos = itv.lower();
					int maxPos = itv.upper()+1;

					if(minPos < 0)
						minPos = 0;
					if(maxPos > maxBscanWidth)
						maxPos = maxBscanWidth;

					fieldMat(cv::Range(static_cast<int>(bscan), static_cast<int>(bscan+1)), cv::Range(minPos, maxPos)) = static_cast<uint8_t>(pos);
				}
			}
		}
	}

	return CppFW::CVMatTreeStructBin::writeBin(filename, tree);
}
