#ifndef MARKERTREE_H
#define MARKERTREE_H

#include <boost/property_tree/ptree_fwd.hpp>

class MarkerTree
{
	boost::property_tree::ptree* const markerPTree = nullptr;

public:
	boost::property_tree::ptree* getPTree()                         { return markerPTree; }
	const boost::property_tree::ptree* getPTree() const             { return markerPTree; }
};

#endif // MARKERTREE_H
