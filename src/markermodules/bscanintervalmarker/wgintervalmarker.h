#ifndef WGINTERVALMARKER_H
#define WGINTERVALMARKER_H

#include <QWidget>

class QToolBox;

class BScanIntervalMarker;
class IntervalMarker;

class WGIntervalMarker : public QWidget
{
	Q_OBJECT;

	BScanIntervalMarker* parent;

	void addMarkerCollection(const IntervalMarker& markers, QToolBox* toolbox);

public:
	WGIntervalMarker(BScanIntervalMarker* parent);
	~WGIntervalMarker();
};

#endif // WGINTERVALMARKER_H
