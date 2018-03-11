#ifndef DEFINEDINTERVALLMARKER_H
#define DEFINEDINTERVALLMARKER_H

#include <data_structure/intervalmarker.h>
#include <map>


#include <QObject>

class DefinedIntervalMarker : QObject
{
	Q_OBJECT
public:
	typedef std::map<std::string, IntervalMarker> IntervallMarkerMap;

	static DefinedIntervalMarker& getInstance()                     { static DefinedIntervalMarker instance; return instance; }

	const IntervallMarkerMap& getIntervallMarkerMap() const         { return intervallMarkerMap; }

private:


	IntervallMarkerMap intervallMarkerMap;

	DefinedIntervalMarker();
	void addMarker(IntervalMarker&& marker);
};

#endif // DEFINEDINTERVALLMARKER_H
