#pragma once


#include <QObject>

class QPainter;
class QMouseEvent;
class BScanMarkerWidget;
class QKeyEvent;
class QEvent;
class QToolBar;

class BScanMarkerManager;


namespace OctData
{
	class Series;
};


class BscanMarkerBase : public QObject
{
	Q_OBJECT
public:
	BscanMarkerBase(BScanMarkerManager* markerManager) : markerManager(markerManager) {}
	virtual ~BscanMarkerBase()                                      {}
	
	virtual void drawMarker(QPainter&, BScanMarkerWidget*)          {}
	virtual bool drawBScan() const                                  { return true;  }
	
	virtual bool mouseMoveEvent   (QMouseEvent*, BScanMarkerWidget*) { return false; } // when return true, a update is necessary
	virtual bool mousePressEvent  (QMouseEvent*, BScanMarkerWidget*) { return false; } // when return true, a update is necessary
	virtual bool mouseReleaseEvent(QMouseEvent*, BScanMarkerWidget*) { return false; } // when return true, a update is necessary
	
	virtual bool keyPressEvent    (QKeyEvent*  , BScanMarkerWidget*) { return false; }
	virtual bool leaveWidgetEvent (QEvent*     , BScanMarkerWidget*) { return false; }
	
	virtual QToolBar* createToolbar(QObject*)                        { return nullptr; }
	
protected:
	BScanMarkerManager* const markerManager;
	
	int getActBScan() const;
	int getBScanWidth() const;
	
	const OctData::Series* getSeries() const;
	
	
};

