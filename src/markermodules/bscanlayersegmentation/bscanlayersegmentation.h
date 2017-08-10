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
	friend class BScanLayerSegPTree;
	friend class LayerSegmentationIO;

	struct BScanSegData
	{
		OctData::Segmentationlines lines;
		bool filled = false;
	};

	std::vector<BScanSegData> lines;
	OctData::Segmentationlines::SegmentlineType actEditType = OctData::Segmentationlines::SegmentlineType::ILM;

	void resetMarkers(const OctData::Series* series);

	QWidget* widgetPtr2WGLayerSeg = nullptr;


	EditBase  * actEditMethod    = nullptr;
	EditSpline* editMethodSpline = nullptr;
	EditPen   * editMethodPen    = nullptr;

	void copySegLinesFromOctDataWhenNotFilled();
	void copySegLinesFromOctDataWhenNotFilled(std::size_t bscan);

	void copySegLinesFromOctData();
	void copySegLinesFromOctData(std::size_t bscan);

	std::size_t getMaxBscanWidth() const;
public:
	enum class SegMethod { None, Pen, Spline };

	BScanLayerSegmentation(OctMarkerManager* markerManager);
	~BScanLayerSegmentation();

	virtual void drawMarker(QPainter& painter, BScanMarkerWidget* widget, const QRect& /*drawrect*/) const override;

	virtual RedrawRequest mouseMoveEvent   (QMouseEvent*, BScanMarkerWidget*) override;
	virtual RedrawRequest mousePressEvent  (QMouseEvent*, BScanMarkerWidget*) override;
	virtual RedrawRequest mouseReleaseEvent(QMouseEvent*, BScanMarkerWidget*) override;


	virtual void saveState(boost::property_tree::ptree& markerTree)  override;
	virtual void loadState(boost::property_tree::ptree& markerTree)  override;


	virtual void setActBScan(std::size_t bscan) override;

	virtual bool keyPressEvent    (QKeyEvent*  , BScanMarkerWidget*) override;

	virtual QWidget* getWidget   ()          override               { return widgetPtr2WGLayerSeg; }

	virtual void newSeriesLoaded(const OctData::Series* series, boost::property_tree::ptree& ptree) override;

	OctData::Segmentationlines::SegmentlineType getActEditSeglineType() const { return actEditType; }


	void setActEditLinetype(OctData::Segmentationlines::SegmentlineType type);

	void setSegMethod(SegMethod method);
	SegMethod getSegMethod() const;

	bool saveSegmentation2Bin(const std::string& filename);
	void copyAllSegLinesFromOctData();

	void setIconsToSimple(int size);
signals:
	void segMethodChanged();
};

#endif // BSCANLAYERSEGMENTATION_H
