#ifndef WGOCTMARKERS_H
#define WGOCTMARKERS_H


#include <QWidget>

class PaintMarkerModel;
class QTableView;

class WGOctMarkers : public QWidget
{
	Q_OBJECT

	QTableView* view;
	PaintMarkerModel* model;

public:
	WGOctMarkers(PaintMarkerModel* model);

};

#endif // WGOCTMARKERS_H
