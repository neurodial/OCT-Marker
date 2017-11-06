#ifndef OBJECTSMARKERPTREE_H
#define OBJECTSMARKERPTREE_H

#include <boost/property_tree/ptree_fwd.hpp>

class Objectsmarker;

class ObjectsMarkerPTree
{

public:
	static bool parsePTree(const boost::property_tree::ptree& ptree,       Objectsmarker* markerManager);
	static void fillPTree (      boost::property_tree::ptree& ptree, const Objectsmarker* markerManager);

};

#endif // OBJECTSMARKERPTREE_H
