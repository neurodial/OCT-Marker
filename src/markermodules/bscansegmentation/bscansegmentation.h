#ifndef BSCANSEGMENTATION_H
#define BSCANSEGMENTATION_H


#include "../bscanmarkerbase.h"
#include "configdata.h"

#include <boost/icl/interval_map.hpp>
#include <data_structure/intervalmarker.h>
#include <vector>

#include <QPoint>

class QAction;
class WGSegmentation;

namespace cv { class Mat; }


class BScanSegLocalOp;
class BScanSegLocalOpPaint;
class BScanSegLocalOpThreshold;
class BScanSegLocalOpOperation;


class BScanSegmentation : public BscanMarkerBase
{
	Q_OBJECT

	// enum class PaintMethod { Disc, Quadrat };

	friend class BScanSegmentationPtree;
	friend class BScanSegLocalOp;
	
	typedef std::vector<cv::Mat*> SegMats;

	bool inWidget = false;
	QPoint mousePoint;

	// Local operation data
	BScanSegmentationMarker::LocalMethod   localMethod = BScanSegmentationMarker::LocalMethod::Paint;

	BScanSegLocalOp*          actLocalOperator = nullptr;
	BScanSegLocalOpPaint*     localOpPaint     = nullptr;
	BScanSegLocalOpThreshold* localOpThreshold = nullptr;
	BScanSegLocalOpOperation* localOpOperation = nullptr;


	bool paint = false;


	WGSegmentation* widget = nullptr;
	QWidget* widgetPtr2WGSegmentation = nullptr;
	
	SegMats segments;

	void clearSegments();

	template<typename T>
	void drawSegmentLine(QPainter&, int factor, const QRect&) const;

	void transformCoordWidget2Mat(int xWidget, int yWidget, int factor, int& xMat, int& yMat);
	

	bool startOnCoord(int x, int y, int factor);

	bool setOnCoord(int x, int y, int factor);
	bool paintOnCoord(cv::Mat* map, int xD, int yD);
	BScanSegmentationMarker::internalMatType valueOnCoord(int x, int y);

	QRect getWidgetPaintSize(const QPoint& p1, const QPoint& p2, int factor);



public:

	BScanSegmentation(BScanMarkerManager* markerManager);
	virtual ~BScanSegmentation();

	QToolBar* createToolbar(QObject* parent) override;
	virtual QWidget* createWidget(QWidget*)  override;
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

	std::size_t getNumBScans() const                                { return segments.size(); }
	
	virtual void newSeriesLoaded(const OctData::Series* series, boost::property_tree::ptree& markerTree) override;

	void initBScanFromThreshold(const BScanSegmentationMarker::ThresholdData& data);
	void initSeriesFromThreshold(const BScanSegmentationMarker::ThresholdData& data);

	BScanSegmentationMarker::LocalMethod getLocalMethod() const     { return localMethod; }


	BScanSegLocalOpPaint*     getLocalOpPaint    ()                 { return localOpPaint    ; }
	BScanSegLocalOpThreshold* getLocalOpThreshold()                 { return localOpThreshold; }
	BScanSegLocalOpOperation* getLocalOpOperation()                 { return localOpOperation; }

public slots:
	virtual void erodeBScan();
	virtual void dilateBScan();
	virtual void opencloseBScan();
	virtual void medianBScan();

	virtual void setLocalMethod(BScanSegmentationMarker::LocalMethod method);

	virtual void updateCursor();

signals:
	void paintArea0Selected(bool = true);
	void paintArea1Selected(bool = true);
	void paintAutoAreaSelected(bool = true);

	void localOperatorSizeChanged(int size);

};

#endif // BSCANSEGMENTATION_H
