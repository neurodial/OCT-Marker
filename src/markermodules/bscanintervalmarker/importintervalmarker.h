#ifndef IMPORTINTERVALMARKER_H
#define IMPORTINTERVALMARKER_H

#include <string>

class BScanIntervalMarker;
class IntervalMarker;


class ImportIntervalMarker
{
public:
	static bool importBin(BScanIntervalMarker* markerManager, const std::string& filename);
	static bool exportBin(BScanIntervalMarker* markerManager, const std::string& filename);
};

#endif // IMPORTINTERVALMARKER_H
