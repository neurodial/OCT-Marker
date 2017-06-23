#ifndef BSCANLAYERSEGMENTATION_H
#define BSCANLAYERSEGMENTATION_H

#include<octdata/datastruct/segmentationlines.h>

#include "../bscanmarkerbase.h"

#include<data_structure/point2d.h>

class QWidget;

class EditBase;
class EditSpline;
class EditPen;

class BScanLayerSegmentation : public BscanMarkerBase
{
	Q_OBJECT

	friend class EditBase;
/*
	struct SegPoint
	{
		SegPoint() : x(0), y(0) {}
		SegPoint(std::size_t x, double y) : x(x), y(y) {}

		std::size_t x;
		double      y;
	};*/
	std::vector<OctData::Segmentationlines> lines;
	OctData::Segmentationlines::SegmentlineType actEditType = OctData::Segmentationlines::SegmentlineType::ILM;

	void resetMarkers(const OctData::Series* series);
/*
	SegPoint lastPoint;
	bool paintSegLine = false;*/

// 	SegPoint calcPoint(int x, int y, double scaleFactor, int bscanWidth);

// 	void setLinePoint2Point(const SegPoint& p1, const SegPoint& p2, OctData::Segmentationlines::Segmentline& segLine);

// 	QRect getWidgetPaintSize(const SegPoint& p1, const SegPoint& p2, double scaleFactor);


	QWidget* widgetPtr2WGLayerSeg = nullptr;

// 	void splineTest();

// 	std::vector<Point2D> polygon;
// 	std::vector<double> interpolated;

	EditBase  * actEditMethod    = nullptr;
	EditSpline* editMethodSpline = nullptr;
	EditPen   * editMethodPen    = nullptr;

public:
	BScanLayerSegmentation(OctMarkerManager* markerManager);
	~BScanLayerSegmentation();

	virtual void drawMarker(QPainter& painter, BScanMarkerWidget* widget, const QRect& /*drawrect*/) const override;

	virtual RedrawRequest mouseMoveEvent   (QMouseEvent*, BScanMarkerWidget*) override;
	virtual RedrawRequest mousePressEvent  (QMouseEvent*, BScanMarkerWidget*) override;
	virtual RedrawRequest mouseReleaseEvent(QMouseEvent*, BScanMarkerWidget*) override;

	virtual bool keyPressEvent    (QKeyEvent*  , BScanMarkerWidget*) override;

	virtual QWidget* getWidget   ()          override               { return widgetPtr2WGLayerSeg; }

	virtual void newSeriesLoaded(const OctData::Series* series, boost::property_tree::ptree& ptree) override;


	void copySegLinesFromOctData();

	void setActEditLinetype(OctData::Segmentationlines::SegmentlineType type);
};

#endif // BSCANLAYERSEGMENTATION_H
