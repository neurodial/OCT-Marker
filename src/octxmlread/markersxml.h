#ifndef MARKERSXML_H
#define MARKERSXML_H

#include <string>

class MarkerManager;

class MarkersXML
{
public:
	static void readXML(MarkerManager* manager, std::string filename);
	static void writeXML(MarkerManager* manager, std::string filename);
};

#endif // MARKERSXML_H
