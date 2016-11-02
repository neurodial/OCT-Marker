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


class BScanSegmentation : public BscanMarkerBase
{
	Q_OBJECT

	// enum class PaintMethod { Disc, Quadrat };

	friend class BScanSegmentationPtree;
	
	typedef std::vector<cv::Mat*> SegMats;

	bool inWidget = false;
	QPoint mousePoint;

	// Local operation data
	BScanSegmentationMarker::LocalMethod   localMethod;
	BScanSegmentationMarker::ThresholdData localThresholdData;
	BScanSegmentationMarker::Operation     localOperation;
	BScanSegmentationMarker::PaintData     localPaintData;


	// PaintMethod paintMethod = PaintMethod::Disc;
	
	bool paint = false;
	int localOperatorSize = 10;

	BScanSegmentationMarker::internalMatType paintValue = BScanSegmentationMarker::markermatInitialValue;
	// bool autoPaintValue = true;

	WGSegmentation* widget = nullptr;
	QWidget* widgetPtr2WGSegmentation = nullptr;
	
	SegMats segments;

	void clearSegments();

	template<typename T>
	void drawSegmentLine(QPainter&, int factor, const QRect&) const;

	void transformCoordWidget2Mat(int xWidget, int yWidget, int factor, int& xMat, int& yMat);
	

	bool startOnCoord(int x, int y, int factor);
	bool startOnCoordPaint(int x, int y, int factor);
	bool startOnCoordOperation(int x, int y, int factor);
	bool startOnCoordThreshold(int x, int y, int factor);


	bool setOnCoord(int x, int y, int factor);
	bool paintOnCoord(cv::Mat* map, int xD, int yD);
	BScanSegmentationMarker::internalMatType valueOnCoord(int x, int y, int factor);

	QRect getWidgetPaintSize(const QPoint& p1, const QPoint& p2, int factor);

	void drawMarkerPaint(QPainter& painter, const QPoint& centerDrawPoint, int factor) const;
	void drawMarkerOperation(QPainter& painter, const QPoint& centerDrawPoint, int factor) const;
	void drawMarkerThreshold(QPainter& painter, const QPoint& centerDrawPoint, int factor) const;

public:

	BScanSegmentation(BScanMarkerManager* markerManager);
	virtual ~BScanSegmentation();

	QToolBar* createToolbar(QObject* parent) override;
	virtual QWidget* createWidget(QWidget*)  override;
	virtual QWidget* getWidget   ()          override               { return widgetPtr2WGSegmentation; }
	
	virtual void drawMarker(QPainter&, BScanMarkerWidget*, const QRect&) const override;
	virtual bool drawBScan() const                         override { return true;  }
	
	virtual RedrawRequest mouseMoveEvent   (QMouseEvent*, BScanMarkerWidget*) override;
	virtual bool mousePressEvent  (QMouseEvent*, BScanMarkerWidget*) override;
	virtual bool mouseReleaseEvent(QMouseEvent*, BScanMarkerWidget*) override;
	virtual bool keyPressEvent    (QKeyEvent*  , BScanMarkerWidget*) override;
	virtual bool leaveWidgetEvent (QEvent*     , BScanMarkerWidget*) override;

	virtual void saveState(boost::property_tree::ptree& markerTree)  override;
	virtual void loadState(boost::property_tree::ptree& markerTree)  override;

	std::size_t getNumBScans() const                                { return segments.size(); }
	
	virtual void newSeriesLoaded(const OctData::Series* series, boost::property_tree::ptree& markerTree) override;



	void initBScanFromThreshold(const BScanSegmentationMarker::ThresholdData& data);
	void initSeriesFromThreshold(const BScanSegmentationMarker::ThresholdData& data);

	int getLocalOperatorSize() const                                { return localOperatorSize; }

	virtual void setLocalPaintData(const BScanSegmentationMarker::PaintData& data);
	virtual void setLocalOperation(const BScanSegmentationMarker::Operation& data);
	virtual void setLocalThreshold(const BScanSegmentationMarker::ThresholdData& data);

public slots:
	void setLocalOperatorSize(int size);
	virtual void erodeBScan();
	virtual void dilateBScan();
	virtual void opencloseBScan();
	virtual void medianBScan();

	virtual void setLocalMethod(BScanSegmentationMarker::LocalMethod method);

signals:
	void paintArea0Selected(bool = true);
	void paintArea1Selected(bool = true);
	void paintAutoAreaSelected(bool = true);

	void localOperatorSizeChanged(int size);
	
private slots:
	
//	virtual void paintArea0Slot()                                   { paintValue  = BScanSegmentationMarker::paintArea0Value; autoPaintValue = false; paintArea0Selected(); }
//	virtual void paintArea1Slot()                                   { paintValue  = BScanSegmentationMarker::paintArea1Value; autoPaintValue = false; paintArea1Selected(); }
//	virtual void autoAddRemoveArea()                                { autoPaintValue = true; paintAutoAreaSelected(); }
	// virtual void setPaintRadius(int r)                              { paintRadius = r; }
	
//	virtual void initFromSegmentline();
//	virtual void initFromThreshold();

/*
	virtual void setPaintMethodDisc();
	virtual void setPaintMethodQuadrat();*/
};

#endif // BSCANSEGMENTATION_H
