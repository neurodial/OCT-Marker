#ifndef BSCANQUALITYMARKER_H
#define BSCANQUALITYMARKER_H

#include "bscanmarkerbase.h"

#include <boost/icl/interval_map.hpp>
#include <data_structure/intervallmarker.h>

class BScanQualityMarker : public BscanMarkerBase
{
public:

	typedef IntervallMarker::Marker Marker;
	typedef boost::icl::interval_map<int, Marker, boost::icl::partial_enricher> MarkerMap;

	enum class Method { Paint, Fill };
	
	Method getMarkerMethod() const                              { return markerMethod; }
	
private:
	
	Marker           aktMarker;
	bool             dataChanged = false;

	Method markerMethod = Method::Paint;

	std::vector<MarkerMap> markers;

};

#endif // BSCANQUALITYMARKER_H
