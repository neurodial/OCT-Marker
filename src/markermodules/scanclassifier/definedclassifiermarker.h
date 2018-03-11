#ifndef DEFINEDCLASSIFIERMARKER_H
#define DEFINEDCLASSIFIERMARKER_H

#include<QObject>

#include"classifiermarker.h"

class DefinedClassifierMarker : QObject
{
	Q_OBJECT
public:
	typedef std::map<std::string, ClassifierMarker> ClassifierMarkerMap;

	static DefinedClassifierMarker& getInstance()                   { static DefinedClassifierMarker instance; return instance; }


private:
	DefinedClassifierMarker();

	ClassifierMarkerMap volumeMarkers;
	ClassifierMarkerMap bscanMarkers;

	void addMarker(ClassifierMarkerMap& map, ClassifierMarker&& marker);
};

#endif // DEFINEDCLASSIFIERMARKER_H
