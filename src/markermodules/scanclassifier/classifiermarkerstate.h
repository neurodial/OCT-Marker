#ifndef CLASSIFIERMARKERSTATE_H
#define CLASSIFIERMARKERSTATE_H

#include"classifiermarker.h"

#include <boost/property_tree/ptree_fwd.hpp>


class ClassifierMarkerState
{
	const ClassifierMarker& cassifierMarker;

	std::vector<bool> markerStatus;

public:
	explicit ClassifierMarkerState(const ClassifierMarker& marker);

	void reset();

	void setStatusId(std::size_t id, bool value)                    { markerStatus.at(id) = value; }
	bool getStatusId(std::size_t id)                          const { return markerStatus.at(id); }

	bool parsePTree(const boost::property_tree::ptree& ptree);
	void fillPTree (      boost::property_tree::ptree& ptree);
};

#endif // CLASSIFIERMARKERSTATE_H
