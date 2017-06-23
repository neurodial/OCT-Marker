#ifndef EDITBASE_H
#define EDITBASE_H

#include<octdata/datastruct/segmentationlines.h>

class QMouseEvent;
class QPainter;
class QRect;
class BScanMarkerWidget;

class BScanLayerSegmentation;

class EditBase
{
	BScanLayerSegmentation* base;
public:
	EditBase(BScanLayerSegmentation* base) : base(base) {}

	virtual void drawMarker(QPainter& /*painter*/, BScanMarkerWidget* /*widget*/, const QRect& /*drawrect*/) const {};

	virtual bool mouseMoveEvent   (QMouseEvent*, BScanMarkerWidget*) { return false; };
	virtual bool mousePressEvent  (QMouseEvent*, BScanMarkerWidget*) { return false; };
	virtual bool mouseReleaseEvent(QMouseEvent*, BScanMarkerWidget*) { return false; };

	virtual void segLineChanged(OctData::Segmentationlines::Segmentline& segLine) {};

};

#endif // EDITBASE_H
