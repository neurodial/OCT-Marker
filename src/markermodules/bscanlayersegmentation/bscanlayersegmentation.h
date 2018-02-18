#ifndef BSCANLAYERSEGMENTATION_H
#define BSCANLAYERSEGMENTATION_H

#include<octdata/datastruct/segmentationlines.h>

#include "../bscanmarkerbase.h"

#include<data_structure/point2d.h>

class QWidget;

class EditBase;
class EditSpline;
class EditPen;
class Colormap;
class ThicknessmapLegend;

class BScanLayerSegmentation : public BscanMarkerBase
{
	Q_OBJECT

	friend class EditBase;
	friend class BScanLayerSegPTree;
	friend class LayerSegmentationIO;

public:
	struct BScanSegData
	{
		OctData::Segmentationlines lines;
		bool filled = false;
	};

	class ThicknessmapConfig
	{
		friend class BScanLayerSegmentation;
		Colormap* colormap = nullptr;
	public:
		OctData::Segmentationlines::SegmentlineType upperLayer;
		OctData::Segmentationlines::SegmentlineType lowerLayer;

		void setUpperColorLimit(double thickness);
		void setYellowColor();
		void setHSVColor();
	};

	enum class SegMethod { None, Pen, Spline };

	BScanLayerSegmentation(OctMarkerManager* markerManager);
	~BScanLayerSegmentation();

	virtual void drawMarker(QPainter& painter, BScanMarkerWidget* widget, const QRect& /*drawrect*/) const override;
	virtual bool drawSLOOverlayImage(const cv::Mat& sloImage, cv::Mat& outSloImage, double alpha) const override;

	virtual RedrawRequest mouseMoveEvent   (QMouseEvent*, BScanMarkerWidget*) override;
	virtual RedrawRequest mousePressEvent  (QMouseEvent*, BScanMarkerWidget*) override;
	virtual RedrawRequest mouseReleaseEvent(QMouseEvent*, BScanMarkerWidget*) override;
	virtual void contextMenuEvent(QContextMenuEvent* event) override;

	virtual void saveState(boost::property_tree::ptree& markerTree)  override;
	virtual void loadState(boost::property_tree::ptree& markerTree)  override;


	virtual void setActBScan(std::size_t bscan) override;

	virtual bool keyPressEvent    (QKeyEvent*  , BScanMarkerWidget*) override;

	virtual QWidget* getWidget   ()          override               { return widgetPtr2WGLayerSeg; }
	virtual QWidget* getSloLegendWidget()    override;

	virtual void newSeriesLoaded(const OctData::Series* series, boost::property_tree::ptree& ptree) override;

	OctData::Segmentationlines::SegmentlineType getActEditSeglineType() const { return actEditType; }


	void setActEditLinetype(OctData::Segmentationlines::SegmentlineType type);

	void setSegMethod(SegMethod method);
	SegMethod getSegMethod() const;

	bool saveSegmentation2Bin(const std::string& filename);
	void copyAllSegLinesFromOctData();

	void setIconsToSimple(int size);

	bool isSegmentationLinesVisible()                         const { return showSegmentationlines; }

	ThicknessmapConfig& getThicknessmapConfig() { return thicknessmapConfig; }

private:
	std::vector<BScanSegData> lines;
	OctData::Segmentationlines::SegmentlineType actEditType = OctData::Segmentationlines::SegmentlineType::ILM;

	void resetMarkers(const OctData::Series* series);

	QWidget* widgetPtr2WGLayerSeg = nullptr;
	ThicknessmapLegend* legendWG = nullptr;


	EditBase  * actEditMethod    = nullptr;
	EditSpline* editMethodSpline = nullptr;
	EditPen   * editMethodPen    = nullptr;

	ThicknessmapConfig thicknessmapConfig;
// 	Colormap* thicknessmapColor = nullptr;

	bool showSegmentationlines = true;

	cv::Mat* thicknesMapImage = nullptr;

	void copySegLinesFromOctDataWhenNotFilled();
	void copySegLinesFromOctDataWhenNotFilled(std::size_t bscan);

	void copySegLinesFromOctData();
	void copySegLinesFromOctData(std::size_t bscan);

	std::size_t getMaxBscanWidth() const;
signals:
	void segMethodChanged();

public slots:
	void setSegmentationLinesVisible(bool visible);

	void generateThicknessmap();
};

#endif // BSCANLAYERSEGMENTATION_H
