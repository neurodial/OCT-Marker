#ifndef BSCANQUALITYMARKER_H
#define BSCANQUALITYMARKER_H

#include "../bscanmarkerbase.h"

#include <boost/icl/interval_map.hpp>
#include <data_structure/intervalmarker.h>
#include <vector>

#include <QPoint>

class QAction;
class QToolBar;
class QActionGroup;
class QWidget;


class BScanIntervalMarker : public BscanMarkerBase
{
	Q_OBJECT
public:
	BScanIntervalMarker(OctMarkerManager* markerManager);
	~BScanIntervalMarker();

	typedef IntervalMarker::Marker Marker;
	typedef boost::icl::interval_map<int, Marker, boost::icl::partial_enricher> MarkerMap;

	enum class Method { Paint, Fill };
	
	Method getMarkerMethod() const                                  { return markerMethod; }
	const std::vector<QAction*>& getMarkerMethodActions()     const { return markerMethodActions; }
	
	void setMarker(int x1, int x2)                                       ;
	void setMarker(int x1, int x2, const Marker& type)                   ;
	void setMarker(int x1, int x2, const Marker& type, std::size_t bscan);

	void fillMarker(int x)                                          { fillMarker(x, actMarker); }
	void fillMarker(int x, const Marker& type);
	
	const Marker& getActMarker() const                              { return actMarker; }
	
	QToolBar* createToolbar(QObject* parent) override;
	virtual QWidget* getWidget   ()          override               { return widgetPtr2WGIntevalMarker; }
	
	virtual void drawBScanSLOLine(QPainter&, int /*bscanNr*/, const OctData::CoordSLOpx& /*start_px*/, const OctData::CoordSLOpx& /*end_px*/, SLOImageWidget*) const override;
	virtual void drawMarker(QPainter&, BScanMarkerWidget*, const QRect&) const override;
	virtual bool drawBScan() const                         override { return true;  }
	
	virtual RedrawRequest mouseMoveEvent   (QMouseEvent*, BScanMarkerWidget*) override;
	virtual RedrawRequest mousePressEvent  (QMouseEvent*, BScanMarkerWidget*) override;
	virtual RedrawRequest mouseReleaseEvent(QMouseEvent*, BScanMarkerWidget*) override;

	virtual bool toolTipEvent     (QEvent*     , BScanMarkerWidget*) override;
	virtual bool keyPressEvent    (QKeyEvent*  , BScanMarkerWidget*) override;
	virtual bool leaveWidgetEvent (QEvent*     , BScanMarkerWidget*) override;
	
	virtual void saveState(boost::property_tree::ptree& markerTree)  override;
	virtual void loadState(boost::property_tree::ptree& markerTree)  override;
	
	const MarkerMap& getMarkers() const                             { return getMarkers(getActBScanNr()); }
	const MarkerMap& getMarkers(std::size_t bscan) const            { if(actCollection && bscan < actCollection->markers.size()) return actCollection->markers[bscan]; return nullMarkerMap; }
	const MarkerMap& getMarkers(const std::string& collection, std::size_t bscan) const;

	std::size_t getNumBScans() const                                { if(actCollection) return actCollection->markers.size(); return 0; }

	virtual void newSeriesLoaded(const OctData::Series* series, boost::property_tree::ptree& markerTree) override;


public slots:
	bool setMarkerCollection(const std::string& internalName);

	virtual void chooseMarkerID(int id);
	virtual void chooseMethodID(int id)                             { markerMethod = static_cast<Method>(id); markerMethodChanged(id); }
	virtual void chooseMethodID(Method id)                          { markerMethod = id; markerMethodChanged(static_cast<int>(id)); }


signals:
	void markerIdChanged(int id);
	void markerMethodChanged(int id);
	
private:
	
	QPoint clickPos;
	QPoint mousePos;

	bool markerActiv = false;
	bool mouseInWidget = false;
	
// 	QAction* fillMarkerAction  = nullptr;
// 	QAction* paintMarkerAction = nullptr;
	
	Marker           actMarker;
	bool             dataChanged = false;

	QWidget* widgetPtr2WGIntevalMarker = nullptr;

	Method markerMethod = Method::Paint;
	std::vector<QAction*> markerMethodActions;

	struct MarkersCollectionData
	{
		std::vector<MarkerMap> markers;
		const IntervalMarker*  markerCollection = nullptr;
	};

	MarkersCollectionData* actCollection = nullptr;
// 	std::vector<MarkerMap>* actMarkerMap = nullptr;
// 	IntervalMarker* markerCollection = nullptr;

	MarkerMap nullMarkerMap; // TODO


	typedef std::map<std::string, MarkersCollectionData> MarkersCollectionsDataList;
	MarkersCollectionsDataList markersCollectionsData;

	static std::size_t getMarkerMapSize(const MarkersCollectionData* collection) { if(collection) return collection->markers.size(); return 0; }

	void createMarkerMethodActions();
	QRect getWidgetPaintSize(const QPoint& p1, const QPoint& p2, double factor, const QPoint* p3 = nullptr);
// 	void addMarkerCollection2Toolbar(const IntervalMarker& markers, QToolBar& markerToolbar, QActionGroup& actionGroupMarker, std::vector<QAction*>& actionList, QObject* parent);
	void resetMarkers(const OctData::Series* series);
};

#endif // BSCANQUALITYMARKER_H
