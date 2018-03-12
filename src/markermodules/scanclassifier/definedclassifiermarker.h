#ifndef DEFINEDCLASSIFIERMARKER_H
#define DEFINEDCLASSIFIERMARKER_H

#include<QObject>

#include"classifiermarker.h"

class DefinedClassifierMarker : QObject
{
	Q_OBJECT
public:
	typedef std::vector<ClassifierMarker> ClassifierMarkerMap;

	static DefinedClassifierMarker& getInstance()                   { static DefinedClassifierMarker instance; return instance; }

	const ClassifierMarkerMap& getVolumeMarkers()             const { return volumeMarkers; }
	const ClassifierMarkerMap& getBscanMarkers ()             const { return bscanMarkers ; }

private:
	DefinedClassifierMarker();

	ClassifierMarkerMap volumeMarkers;
	ClassifierMarkerMap bscanMarkers ;

	void addMarker(ClassifierMarkerMap& map, ClassifierMarker&& marker);
};

#endif // DEFINEDCLASSIFIERMARKER_H
