#ifndef EDITBASE_H
#define EDITBASE_H

#include<octdata/datastruct/segmentationlines.h>

#include"../bscanmarkerbase.h"

class QMouseEvent;
class QPainter;
class QRect;
class QContextMenuEvent;

class BScanLayerSegmentation;
class BScanMarkerWidget;
class ScaleFactor;

class EditBase
{
	BScanLayerSegmentation* parent;
public:
	EditBase(BScanLayerSegmentation* parent) : parent(parent) {}

	virtual void drawMarker(QPainter& /*painter*/, BScanMarkerWidget* /*widget*/, const QRect& /*drawrect*/, const ScaleFactor& /*scaleFactor*/) const {};

	virtual BscanMarkerBase::RedrawRequest mouseMoveEvent   (QMouseEvent*, BScanMarkerWidget*) { return BscanMarkerBase::RedrawRequest(); };
	virtual BscanMarkerBase::RedrawRequest mousePressEvent  (QMouseEvent*, BScanMarkerWidget*) { return BscanMarkerBase::RedrawRequest(); };
	virtual BscanMarkerBase::RedrawRequest mouseReleaseEvent(QMouseEvent*, BScanMarkerWidget*) { return BscanMarkerBase::RedrawRequest(); };
	virtual void contextMenuEvent(QContextMenuEvent* /*event*/) {}

	virtual bool keyPressEvent(QKeyEvent*, BScanMarkerWidget*)      { return false;}

	virtual void segLineChanged(OctData::Segmentationlines::Segmentline* /*segLine*/) {};

protected:
	int getBScanWidth() const;
	int getBScanHight() const;
	void requestFullUpdate();

	void rangeModified(std::size_t ascanBegin, std::size_t ascanEnd);
};

#endif // EDITBASE_H
