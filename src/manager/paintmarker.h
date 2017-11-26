#ifndef PAINTMARKER_H
#define PAINTMARKER_H


#include <model/paintmarkermodel.h>

class QPaintEvent;
class BScanMarkerWidget;

class PaintMarker
{

	PaintMarkerModel model;

public:

	      PaintMarkerModel& getModel()                              { return model; }
	const PaintMarkerModel& getModel() const                        { return model; }


	void paintMarker(QPaintEvent* event, BScanMarkerWidget*) const;
};

#endif // PAINTMARKER_H
