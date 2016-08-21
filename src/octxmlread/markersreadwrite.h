#ifndef MARKERSXML_H
#define MARKERSXML_H

#include <string>

class MarkerManager;

class MarkersReadWrite
{
public:
	static bool readXML(MarkerManager* manager, std::string filename);
	static void writeXML(MarkerManager* manager, std::string filename);

	static bool readJosn(MarkerManager* manager, std::string filename);
	static void writeJosn(MarkerManager* manager, std::string filename);
};

#endif // MARKERSXML_H
