#ifndef WGINTERVALMARKER_H
#define WGINTERVALMARKER_H

#include <QWidget>

#include <vector>
#include <string>

class QToolBox;

class BScanIntervalMarker;
class IntervalMarker;

class WGIntervalMarker : public QWidget
{
	Q_OBJECT;

	BScanIntervalMarker* parent;

	std::vector<std::string> collectionsInternalNames;

	void addMarkerCollection(const IntervalMarker& markers, QToolBox* toolbox);

public:
	WGIntervalMarker(BScanIntervalMarker* parent);
	~WGIntervalMarker();

private slots:
	void changeIntervalCollection(std::size_t index);
};

#endif // WGINTERVALMARKER_H
