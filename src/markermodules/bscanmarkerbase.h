#pragma once


#include <QObject>
#include <QIcon>

#include <boost/property_tree/ptree_fwd.hpp>

class QPainter;
class QMouseEvent;
class BScanMarkerWidget;
class QKeyEvent;
class QEvent;
class QToolBar;

class BScanMarkerManager;
class SLOImageWidget;


namespace OctData
{
	class Series;
	class CoordSLOpx;
};


class BscanMarkerBase : public QObject
{
	Q_OBJECT
public:
	class RedrawRequest
	{
	public:
		bool redraw = false;
		QRect rect;
	};


	BscanMarkerBase(BScanMarkerManager* markerManager) : markerManager(markerManager) {}
	virtual ~BscanMarkerBase()                                      {}
	
	virtual void drawBScanSLOLine(QPainter&, int /*bscanNr*/, const OctData::CoordSLOpx& /*start_px*/, const OctData::CoordSLOpx& /*end_px*/, SLOImageWidget*) const
	                                                                {}
	virtual void drawMarker(QPainter&, BScanMarkerWidget*, const QRect& /*drawrect*/) const    {}
	virtual bool drawBScan() const                                  { return true;  }
	
	virtual RedrawRequest mouseMoveEvent   (QMouseEvent*, BScanMarkerWidget*) { return RedrawRequest(); } // when return true, a update is necessary
	virtual bool mousePressEvent  (QMouseEvent*, BScanMarkerWidget*) { return false; } // when return true, a update is necessary
	virtual bool mouseReleaseEvent(QMouseEvent*, BScanMarkerWidget*) { return false; } // when return true, a update is necessary
	
	virtual bool keyPressEvent    (QKeyEvent*  , BScanMarkerWidget*) { return false; }
	virtual bool leaveWidgetEvent (QEvent*     , BScanMarkerWidget*) { return false; }
	
	virtual QToolBar* createToolbar(QObject*)                       { return nullptr; }
	virtual QWidget*  createWidget (QWidget*)                       { return nullptr; }
	virtual QWidget*  getWidget ()                                  { return nullptr; }
	
	virtual const QString& getName()                                { return name; }
	virtual const QString& getMarkerId()                            { return id;   }
	virtual const QIcon&   getIcon()                                { return icon; }
	
	virtual void activate(bool);
	virtual void saveState(boost::property_tree::ptree&)            {}
	virtual void loadState(boost::property_tree::ptree&)            {}
	
	virtual void newSeriesLoaded(const OctData::Series*, boost::property_tree::ptree&)
	                                                                {}
signals:
	void enabledToolbar(bool b);
	void requestUpdate();
	
	
protected:
	BScanMarkerManager* const markerManager;
	
	void connectToolBar(QToolBar* toolbar);
	
	int getActBScan() const;
	int getBScanWidth() const;
	int getBScanHight() const;
	int getBScanWidth(int nr) const;
	
	const OctData::Series* getSeries() const;
	
	QString name;
	QString id;
	QIcon  icon;
	bool isActivated = false;
	
};

