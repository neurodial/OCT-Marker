#ifndef BSCANSEGMENTATIONPTREE_H
#define BSCANSEGMENTATIONPTREE_H


#include <boost/property_tree/ptree_fwd.hpp>

class BScanSegmentation;

class BScanSegmentationPtree
{
public:
	static bool parsePTree(const boost::property_tree::ptree& ptree,       BScanSegmentation* markerManager);
	static void fillPTree (      boost::property_tree::ptree& ptree, const BScanSegmentation* markerManager);
};


#endif // BSCANSEGMENTATIONPTREE_H
