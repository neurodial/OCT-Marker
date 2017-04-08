#ifndef WGINTERVALMARKER_H
#define WGINTERVALMARKER_H

#include <QWidget>

#include <vector>
#include <string>

class QToolBox;
class QButtonGroup;

class BScanIntervalMarker;
class IntervalMarker;

class WGIntervalMarker : public QWidget
{
	Q_OBJECT;

	BScanIntervalMarker* parent;
	QToolBox* toolboxCollections = nullptr;

	std::vector<std::string> collectionsInternalNames;
	std::vector<QButtonGroup*> collectionsButtonGroups;

	void addMarkerCollection(const IntervalMarker& markers);
	QWidget* createMarkerToolButtons();

public:
	WGIntervalMarker(BScanIntervalMarker* parent);
	~WGIntervalMarker();

private slots:
	void changeIntervalCollection(std::size_t index);
	void changeMarkerId(std::size_t index);
};

#endif // WGINTERVALMARKER_H
