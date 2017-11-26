#ifndef PAINTMARKER_H
#define PAINTMARKER_H

#include<QObject>

#include <model/paintmarkermodel.h>

class QPaintEvent;
class BScanMarkerWidget;

class PaintMarker : public QObject
{
	Q_OBJECT

	PaintMarkerModel model;

public:
	PaintMarker();

	      PaintMarkerModel& getModel()                              { return model; }
	const PaintMarkerModel& getModel() const                        { return model; }


	void paintMarker(QPaintEvent* event, BScanMarkerWidget*) const;

signals:
	void viewChanged();
};

#endif // PAINTMARKER_H
