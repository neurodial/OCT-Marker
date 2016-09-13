#ifndef MARKERSXML_H
#define MARKERSXML_H

#include <string>

class BScanMarkerManager;

class MarkersReadWrite
{
public:
	static bool readXML(BScanMarkerManager* manager, std::string filename);
	static void writeXML(BScanMarkerManager* manager, std::string filename);

	static bool readJosn(BScanMarkerManager* manager, std::string filename);
	static void writeJosn(BScanMarkerManager* manager, std::string filename);
};

#endif // MARKERSXML_H
