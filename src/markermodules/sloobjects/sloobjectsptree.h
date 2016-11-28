#ifndef SLOOBJECTSPTREE_H
#define SLOOBJECTSPTREE_H

#include <boost/property_tree/ptree_fwd.hpp>

class SloObjectMarker;

class SloObjectsPtree
{
public:
	static bool parsePTree(const boost::property_tree::ptree& ptree,       SloObjectMarker* markerManager);
	static void fillPTree (      boost::property_tree::ptree& ptree, const SloObjectMarker* markerManager);

};

#endif // SLOOBJECTSPTREE_H
