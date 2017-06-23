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
	std::vector<OctData::Segmentationlines> lines;
	OctData::Segmentationlines::SegmentlineType actEditType = OctData::Segmentationlines::SegmentlineType::ILM;

	void resetMarkers(const OctData::Series* series);

	QWidget* widgetPtr2WGLayerSeg = nullptr;


	EditBase  * actEditMethod    = nullptr;
	EditSpline* editMethodSpline = nullptr;
	EditPen   * editMethodPen    = nullptr;

public:
	enum class SegMethod { None, Pen, Spline };

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

	void setSegMethod(SegMethod method);
};

#endif // BSCANLAYERSEGMENTATION_H
