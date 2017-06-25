#ifndef EDITBASE_H
#define EDITBASE_H

#include<octdata/datastruct/segmentationlines.h>

#include"../bscanmarkerbase.h"

class QMouseEvent;
class QPainter;
class QRect;
class BScanMarkerWidget;

class BScanLayerSegmentation;


class EditBase
{
	BScanLayerSegmentation* parent;
public:
	EditBase(BScanLayerSegmentation* parent) : parent(parent) {}

	virtual void drawMarker(QPainter& /*painter*/, BScanMarkerWidget* /*widget*/, const QRect& /*drawrect*/, double scaleFactor) const {};

	virtual BscanMarkerBase::RedrawRequest mouseMoveEvent   (QMouseEvent*, BScanMarkerWidget*) { return BscanMarkerBase::RedrawRequest(); };
	virtual BscanMarkerBase::RedrawRequest mousePressEvent  (QMouseEvent*, BScanMarkerWidget*) { return BscanMarkerBase::RedrawRequest(); };
	virtual BscanMarkerBase::RedrawRequest mouseReleaseEvent(QMouseEvent*, BScanMarkerWidget*) { return BscanMarkerBase::RedrawRequest(); };

	virtual bool keyPressEvent(QKeyEvent*, BScanMarkerWidget*)      { return false;}

	virtual void segLineChanged(OctData::Segmentationlines::Segmentline* segLine) {};

protected:
	int getBScanWidth() const;
	int getBScanHight() const;
	void requestFullUpdate();

};

#endif // EDITBASE_H
