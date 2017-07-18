#ifndef BSCANLAYERSEGPTREE_H
#define BSCANLAYERSEGPTREE_H

#include <boost/property_tree/ptree_fwd.hpp>

class BScanLayerSegmentation;

class BScanLayerSegPTree
{
public:
	static bool parsePTree(const boost::property_tree::ptree& ptree,       BScanLayerSegmentation* markerManager);
	static void fillPTree (      boost::property_tree::ptree& ptree, const BScanLayerSegmentation* markerManager);
};

#endif // BSCANLAYERSEGPTREE_H
