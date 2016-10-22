#ifndef BSCANQUALITYMARKER_H
#define BSCANQUALITYMARKER_H

#include "bscanmarkerbase.h"

#include <boost/icl/interval_map.hpp>
#include <data_structure/intervallmarker.h>
#include <vector>

#include <QPoint>

class QToolBar;
class QAction;

class BScanMarkerManager;

class BScanIntervallMarker : public BscanMarkerBase
{
	Q_OBJECT
public:
	BScanIntervallMarker(BScanMarkerManager* markerManager);

	typedef IntervallMarker::Marker Marker;
	typedef boost::icl::interval_map<int, Marker, boost::icl::partial_enricher> MarkerMap;

	enum class Method { Paint, Fill };
	
	Method getMarkerMethod() const                                  { return markerMethod; }
	
	void setMarker(int x1, int x2)                               ;
	void setMarker(int x1, int x2, const Marker& type)           ;
	void setMarker(int x1, int x2, const Marker& type, int bscan);

	void fillMarker(int x)                                          { fillMarker(x, aktMarker); }
	void fillMarker(int x, const Marker& type);
	
	const Marker& getActMarker() const                              { return aktMarker; }
	
	QToolBar* createToolbar(QObject* parent) override;
	
	virtual void drawMarker(QPainter&, BScanMarkerWidget*) override;
	virtual bool drawBScan() const                         override { return true;  }
	
	virtual bool mouseMoveEvent   (QMouseEvent*, BScanMarkerWidget*) override;
	virtual bool mousePressEvent  (QMouseEvent*, BScanMarkerWidget*) override;
	virtual bool mouseReleaseEvent(QMouseEvent*, BScanMarkerWidget*) override;
	virtual bool keyPressEvent    (QKeyEvent*  , BScanMarkerWidget*) override;
	virtual bool leaveWidgetEvent (QEvent*     , BScanMarkerWidget*) override;
	
	
	const MarkerMap& getMarkers() const                             { return markers.at(getActBScan()); }
	const MarkerMap& getMarkers(int bscan) const                    { return markers.at(bscan); }
	
private slots:
	virtual void chooseMarkerID(int id)                             { aktMarker = IntervallMarker::getInstance().getMarkerFromID(id); }
	virtual void chooseMethodID(int id)                             { markerMethod = static_cast<Method>(id); /* emit(markerMethodChanged(markerMethod)); TODO*/ }
	
	virtual void newSeriesLoaded(const OctData::Series* series);
	
private:
	
	QPoint clickPos;
	QPoint mousePos;

	bool markerActiv = false;
	bool mouseInWidget = false;
	
	std::vector<QAction*> markersActions;
	QAction* fillMarkerAction  = nullptr;
	QAction* paintMarkerAction = nullptr;
	
	Marker           aktMarker;
	bool             dataChanged = false;

	Method markerMethod = Method::Paint;

	std::vector<MarkerMap> markers;

};

#endif // BSCANQUALITYMARKER_H
