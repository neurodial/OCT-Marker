#ifndef BSCANSEGMENTATION_H
#define BSCANSEGMENTATION_H


#include "../bscanmarkerbase.h"
#include "configdata.h"

#include <vector>
#include <boost/icl/interval_map.hpp>

#include <QPoint>

#include <octdata/datastruct/segmentationlines.h>

#include <data_structure/intervalmarker.h>
#include <data_structure/scalefactor.h>

class QAction;
class WGSegmentation;

namespace cv { class Mat; }


class BScanSegLocalOp;
class BScanSegLocalOpPaint;
class BScanSegLocalOpThreshold;
class BScanSegLocalOpThresholdDirection;
class BScanSegLocalOpOperation;
class BScanSegLocalOpNN;

class SimpleCvMatCompress;

class BScanSegmentation : public BscanMarkerBase
{
	Q_OBJECT

	friend class BScanSegmentationPtree;
	friend class BScanSegLocalOp;
	friend class ImportSegmentation;
	
	typedef std::vector<SimpleCvMatCompress*> SegMats;
	enum class ViewMethod { Rect, MarchingSquare };

	bool stateChangedSinceLastSave = false;

	bool inWidget = false;
	QPoint mousePoint;

	// Local operation data
	BScanSegmentationMarker::LocalMethod   localMethod = BScanSegmentationMarker::LocalMethod::None;

	BScanSegLocalOp*                   actLocalOperator          = nullptr;
	BScanSegLocalOpPaint*              localOpPaint              = nullptr;
	BScanSegLocalOpThreshold*          localOpThreshold          = nullptr;
	BScanSegLocalOpThresholdDirection* localOpThresholdDirection = nullptr;
	BScanSegLocalOpOperation*          localOpOperation          = nullptr;
	BScanSegLocalOpNN*                 localOpNN                 = nullptr;


	bool paint = false;
	ViewMethod viewMethod = ViewMethod::Rect;


	WGSegmentation* widget = nullptr;
	QWidget* widgetPtr2WGSegmentation = nullptr;
	
	SegMats segments;
	mutable cv::Mat* actMat = nullptr;
	mutable std::size_t actMatNr = 0;
	QImage areaImage;

	void updateAreaImage(const QRect& rect);
	void updateAreaImage(const RedrawRequest& redraw, const ScaleFactor& factor);

	void clearSegments();
	void createSegments();
	void createSegments(const OctData::Series* series);

	template<typename Painter, typename Transformer>
	void drawSegmentLine(Painter& painter, Transformer& transform, const ScaleFactor& factor, const QRect& rect) const;
	template<typename Painter>
	void drawSegmentLine(Painter& painter, const ScaleFactor& factor, const QRect& rect) const;

	void transformCoordWidget2Mat(int xWidget, int yWidget, const ScaleFactor& factor, int& xMat, int& yMat);
	

	bool startOnCoord(int x, int y, const ScaleFactor& factor);

	bool setOnCoord(int x, int y, const ScaleFactor& factor);
	bool paintOnCoord(cv::Mat* map, int xD, int yD);
	BScanSegmentationMarker::internalMatType valueOnCoord(int x, int y);

	QRect getWidgetPaintSize(const QPoint& p1, const QPoint& p2, const ScaleFactor& factor);

	bool setActMat(std::size_t nr, bool saveOldState = true);
	bool hasActMatChanged() const;

	QString generateTikzCode() const;

public:

	BScanSegmentation(OctMarkerManager* markerManager);
	virtual ~BScanSegmentation();

	QToolBar* createToolbar(QObject* parent) override;
	virtual QWidget* getWidget   ()          override               { return widgetPtr2WGSegmentation; }
	
	virtual void drawMarker(QPainter&, BScanMarkerWidget*, const QRect&) const override;
	virtual bool drawBScan() const                         override { return true;  }
	
	virtual RedrawRequest mouseMoveEvent   (QMouseEvent*, BScanMarkerWidget*) override;
	virtual RedrawRequest mousePressEvent  (QMouseEvent*, BScanMarkerWidget*) override;
	virtual RedrawRequest mouseReleaseEvent(QMouseEvent*, BScanMarkerWidget*) override;
	virtual bool keyPressEvent    (QKeyEvent*  , BScanMarkerWidget*) override;
	virtual bool leaveWidgetEvent (QEvent*     , BScanMarkerWidget*) override;

	virtual void saveState(boost::property_tree::ptree& markerTree)  override;
	virtual void loadState(boost::property_tree::ptree& markerTree)  override;

	virtual void setActBScan(std::size_t bscan)  override           { setActMat(bscan, true); }
	virtual bool hasChangedSinceLastSave() const override           { if(stateChangedSinceLastSave) return true; return hasActMatChanged(); }

	std::size_t getNumBScans() const                                { return segments.size(); }
	
	virtual void newSeriesLoaded(const OctData::Series* series, boost::property_tree::ptree& markerTree) override;

	void initBScanFromThreshold (const BScanSegmentationMarker::ThresholdDirectionData& data);
	void initSeriesFromThreshold(const BScanSegmentationMarker::ThresholdDirectionData& data);


	BScanSegmentationMarker::LocalMethod getLocalMethod() const     { return localMethod; }
	bool swapActMat(SimpleCvMatCompress& otherMat);

	void createUndoStep();


	BScanSegLocalOpThresholdDirection* getLocalOpThresholdDirection() { return localOpThresholdDirection; }
	BScanSegLocalOpThreshold*          getLocalOpThreshold()        { return localOpThreshold; }
	BScanSegLocalOpPaint*              getLocalOpPaint    ()        { return localOpPaint    ; }
	BScanSegLocalOpOperation*          getLocalOpOperation()        { return localOpOperation; }
	BScanSegLocalOpNN*                 getLocalOpNN       ()        { return localOpNN       ; }

	void initSeriesFromSegline(OctData::Segmentationlines::SegmentlineType type);
	void initBScanFromSegline (OctData::Segmentationlines::SegmentlineType type);

public slots:
	virtual void erodeBScan();
	virtual void dilateBScan();
	virtual void opencloseBScan();
	virtual void medianBScan();
	virtual void removeUnconectedAreas();
	virtual void extendLeftRightSpace();

	virtual void seriesRemoveUnconectedAreas();
	virtual void seriesExtendLeftRightSpace();


	virtual void setLocalMethod(BScanSegmentationMarker::LocalMethod method);

	virtual void updateCursor();

	void removeSeriesSegmentation()                                 { createSegments(); requestFullUpdate(); }

	void showTikzCode();

private slots:
	void updateAreaImageSlot();

signals:
	void paintArea0Selected(bool = true);
	void paintArea1Selected(bool = true);
	void paintAutoAreaSelected(bool = true);

	void localOperatorChanged(BScanSegmentationMarker::LocalMethod method);

};

#endif // BSCANSEGMENTATION_H
