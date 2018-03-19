#ifndef BSCANQUALITYMARKER_H
#define BSCANQUALITYMARKER_H

#include "../bscanmarkerbase.h"

#include <boost/icl/interval_map.hpp>
#include <data_structure/intervalmarker.h>
#include"intervalllegend.h"
#include <vector>

#include <QPoint>

class QAction;
class QToolBar;
class QActionGroup;
class QWidget;

class ScaleFactor;
class WidgetOverlayLegend;


class BScanIntervalMarker : public BscanMarkerBase
{
	Q_OBJECT
public:
	typedef IntervalMarker::Marker Marker;
	typedef boost::icl::interval_map<int, Marker, boost::icl::partial_enricher> MarkerMap;

	struct MarkersCollectionData
	{
		std::vector<MarkerMap> markers;
		const IntervalMarker*  markerCollection = nullptr;
	};
	typedef std::map<std::string, MarkersCollectionData> MarkersCollectionsDataList;

	class MarkerCollectionWork
	{
		friend class BScanIntervalMarker;
		MarkersCollectionsDataList::iterator actCollection;
	public:
		MarkerCollectionWork(MarkersCollectionsDataList::iterator actCollection) : actCollection(actCollection) {}
	};

	BScanIntervalMarker(OctMarkerManager* markerManager);
	~BScanIntervalMarker();


	enum class Method { Paint, Fill };
	
	Method getMarkerMethod() const                                  { return markerMethod; }
	const std::vector<QAction*>& getMarkerMethodActions()     const { return markerMethodActions; }
	
	void setMarker(int x1, int x2)                                       ;
	void setMarker(int x1, int x2, const Marker& type)                   ;
	void setMarker(int x1, int x2, const Marker& type, std::size_t bscan);
	void setMarker(int x1, int x2, const Marker& type, std::size_t bscan, MarkerCollectionWork& w);

	void fillMarker(int x)                                          { fillMarker(x, actMarker); }
	void fillMarker(int x, const Marker& type);
	
	const Marker& getActMarker() const                              { return actMarker; }
	
	QToolBar* createToolbar(QObject* parent) override;
	virtual QWidget* getWidget   ()          override               { return widgetPtr2WGIntevalMarker; }
	WidgetOverlayLegend* getSloLegendWidget() override              { return &widgetOverlayLegend; }
	
	virtual bool drawingBScanOnSLO() const   override               { return true; }
	virtual void drawBScanSLOLine  (QPainter&, std::size_t bscanNr, const OctData::CoordSLOpx& start_px, const OctData::CoordSLOpx& end_px   , SLOImageWidget*) const override;
	virtual void drawBScanSLOCircle(QPainter&, std::size_t bscanNr, const OctData::CoordSLOpx& start_px, const OctData::CoordSLOpx& center_px, bool clockwise, SLOImageWidget*) const override;
	virtual void drawMarker(QPainter&, BScanMarkerWidget*, const QRect&) const override;
	virtual bool drawSLOOverlayImage(const cv::Mat& sloImage, cv::Mat& outSloImage, double alpha) const override;
	virtual bool drawBScan() const                         override { return true;  }
	
	virtual RedrawRequest mouseMoveEvent   (QMouseEvent*, BScanMarkerWidget*) override;
	virtual RedrawRequest mousePressEvent  (QMouseEvent*, BScanMarkerWidget*) override;
	virtual RedrawRequest mouseReleaseEvent(QMouseEvent*, BScanMarkerWidget*) override;

	virtual bool toolTipEvent     (QEvent*     , BScanMarkerWidget*) override;
	virtual bool keyPressEvent    (QKeyEvent*  , BScanMarkerWidget*) override;
	virtual bool leaveWidgetEvent (QEvent*     , BScanMarkerWidget*) override;
	
	virtual void saveState(boost::property_tree::ptree& markerTree)  override;
	virtual void loadState(boost::property_tree::ptree& markerTree)  override;

	void setActBScan(std::size_t bscan) override;
	
	const IntervalMarker* getMarkersList(const MarkerCollectionWork& collection) const
	                                                                { if(collection.actCollection == markersCollectionsData.end()) return nullptr; return collection.actCollection->second.markerCollection; }
	const IntervalMarker* getMarkersList() const                    { return getMarkersList(MarkerCollectionWork(actCollection)); }

	const MarkerMap& getMarkers() const                             { return getMarkers(getActBScanNr()); }
	const MarkerMap& getMarkers(std::size_t bscan) const;
	const MarkerMap& getMarkers(const std::string& collection, std::size_t bscan) const;

	virtual bool hasChangedSinceLastSave() const override           { return stateChangedSinceLastSave; }

	std::size_t getNumBScans() const                                { if(actCollectionValid()) return actCollection->second.markers.size(); return 0; }
	const std::string& getActMarkerCollectionInternalName()   const { if(actCollectionValid()) return actCollection->first; static std::string nullString; return nullString; }

	virtual void newSeriesLoaded(const OctData::Series* series, boost::property_tree::ptree& markerTree) override;

	MarkerCollectionWork getMarkerCollection(const std::string& internalName);
	MarkerCollectionWork getActMarkerCollection()                   { return MarkerCollectionWork(actCollection); }

	bool importMarkerFromBin(const std::string& filename);
	void exportMarkerToBin(const std::string& filename);

	uint8_t getTransparency() const                                 { return transparency; }

	std::size_t getMaxBscanWidth() const;

public slots:
	bool setMarkerCollection(const std::string& internalName);

	virtual bool chooseMarkerID(int id);
	virtual void chooseMethodID(int id)                             { markerMethod = static_cast<Method>(id); markerMethodChanged(id); }
	virtual void chooseMethodID(Method id)                          { markerMethod = id; markerMethodChanged(static_cast<int>(id)); }

	virtual void setTransparency(int value)                         { if(value != transparency) {transparency = static_cast<uint8_t>(value); requestFullUpdate();} }


signals:
	void markerIdChanged(int id);
	void markerMethodChanged(int id);
	void markerCollectionChanged(const std::string& internalName);

private:
	
	QPoint clickPos;
	QPoint mousePos;

	bool markerActiv = false;
	bool mouseInWidget = false;
	IntervallLegend widgetOverlayLegend;
	
// 	QAction* fillMarkerAction  = nullptr;
// 	QAction* paintMarkerAction = nullptr;
	
	Marker           actMarker;
	bool             stateChangedSinceLastSave = false;
	bool             stateChangedInActBScan    = false;
	uint8_t          transparency = 60;

	QWidget* widgetPtr2WGIntevalMarker = nullptr;

	Method markerMethod = Method::Paint;
	std::vector<QAction*> markerMethodActions;


	cv::Mat* sloOverlayImage = nullptr;

	MarkerMap nullMarkerMap; // TODO


	MarkersCollectionsDataList markersCollectionsData;
	MarkersCollectionsDataList::iterator actCollection;


	void setMarker(int x1, int x2, const Marker& type, std::size_t bscan, MarkersCollectionsDataList::iterator& collection);

	bool actCollectionValid() const                                 { return actCollection != markersCollectionsData.end(); }

	static std::size_t getMarkerMapSize(const MarkersCollectionData& collection)
	                                                                { return collection.markers.size(); }
	std::size_t getMarkerMapSize()                            const { if(actCollectionValid()) return getMarkerMapSize(actCollection->second); return 0; }

	void createMarkerMethodActions();
	QRect getWidgetPaintSize(const QPoint& p1, const QPoint& p2, const ScaleFactor& factor, const QPoint* p3 = nullptr);
// 	void addMarkerCollection2Toolbar(const IntervalMarker& markers, QToolBar& markerToolbar, QActionGroup& actionGroupMarker, std::vector<QAction*>& actionList, QObject* parent);
	void resetMarkers(const OctData::Series* series);

	void generateSloMap();
};

#endif // BSCANQUALITYMARKER_H
