#pragma once


class QPainter;
class QMouseEvent;

class BscanMarkerBase
{
public:
	virtual void drawMarker(QPainter*)                              {}
	virtual bool drawBScan() const                                  { return true;  }
	
	virtual bool mouseMoveEvent   (QMouseEvent*)                    { return false; } // when return true, a update is necessary
	virtual bool mousePressEvent  (QMouseEvent*)                    { return false; } // when return true, a update is necessary
	virtual bool mouseReleaseEvent(QMouseEvent*)                    { return false; } // when return true, a update is necessary
};

