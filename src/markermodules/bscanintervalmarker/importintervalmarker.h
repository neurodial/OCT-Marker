#ifndef IMPORTINTERVALMARKER_H
#define IMPORTINTERVALMARKER_H

#include <string>

class BScanIntervalMarker;
class IntervalMarker;


class ImportIntervalMarker
{
public:
	static bool importBin(BScanIntervalMarker* markerManager, const IntervalMarker& markerCollection, const std::string& filename);
};

#endif // IMPORTINTERVALMARKER_H
