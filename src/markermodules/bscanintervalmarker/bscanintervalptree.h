#ifndef BSCANINTERVALLPTREE_H
#define BSCANINTERVALLPTREE_H


#include <boost/property_tree/ptree_fwd.hpp>

class BScanIntervalMarker;

class BScanIntervalPTree
{
public:
	static bool parsePTree(const boost::property_tree::ptree& ptree,       BScanIntervalMarker* markerManager);
	static void fillPTree (      boost::property_tree::ptree& ptree, const BScanIntervalMarker* markerManager);
};

#endif // BSCANINTERVALLPTREE_H
