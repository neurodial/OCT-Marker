/*
 * Copyright (c) 2018 Kay Gawlik <kaydev@amarunet.de> <kay.gawlik@beuth-hochschule.de> <kay.gawlik@charite.de>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef BSCANLAYERSEGMENTATION_H
#define BSCANLAYERSEGMENTATION_H

#include<octdata/datastruct/segmentationlines.h>

#include<array>

#include "../bscanmarkerbase.h"

#include<data_structure/point2d.h>
#include "thicknessmaptemplates.h"
#include<array>

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
		std::array<bool, std::tuple_size<OctData::Segmentationlines::SegLinesTypeList>::value> lineModified;
		std::array<bool, std::tuple_size<OctData::Segmentationlines::SegLinesTypeList>::value> lineLoaded;
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
		void setLowerColorLimit(double thickness);
		void setYellowColor();
		void setHSVColor();
	};

	static const std::array<OctData::Segmentationlines::SegmentlineType, 10> keySeglines;

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
	virtual bool hasChangedSinceLastSave() const override;

	virtual bool keyPressEvent    (QKeyEvent*  , BScanMarkerWidget*) override;

	virtual QWidget* getWidget   ()          override               { return widgetPtr2WGLayerSeg; }
	virtual WidgetOverlayLegend* getSloLegendWidget() override;

	virtual void newSeriesLoaded(const OctData::Series* series, boost::property_tree::ptree& ptree) override;

	OctData::Segmentationlines::SegmentlineType getActEditSeglineType() const { return actEditType; }

	void setSegMethod(SegMethod method);
	SegMethod getSegMethod() const;

	bool saveSegmentation2Bin(const std::string& filename);
	void copyAllSegLinesFromOctData();

	void setIconsToSimple(int size);

	bool isSegmentationLinesVisible()                         const { return showSegmentationlines; }
	bool isThicknessmapVisible()                              const { return showThicknessmap; }

	void modifiedSegPart(std::size_t bscan, OctData::Segmentationlines::SegmentlineType segLine, std::size_t start, const std::vector<double>& segPart)
	                                                                { modifiedSegPart(bscan, segLine, start, segPart, true); }

	ThicknessmapConfig& getThicknessmapConfig()                     { return thicknessmapConfig; }
	void setThicknessmapConfig(const ThicknessmapTemplates::Configuration& config);

private:
	OctData::Segmentationlines::Segmentline tempLine;
	std::vector<BScanSegData> lines;
	OctData::Segmentationlines::SegmentlineType actEditType = OctData::Segmentationlines::SegmentlineType::ILM;

	bool highlightLine = false;
	OctData::Segmentationlines::SegmentlineType acthighlightLineType = OctData::Segmentationlines::SegmentlineType::ILM;


	void resetMarkers(const OctData::Series* series);
	void resetMarkers(std::size_t bscanNr);

	QWidget* widgetPtr2WGLayerSeg = nullptr;
	ThicknessmapLegend*  thicknessMapLegend = nullptr;
	WidgetOverlayLegend* legendWG = nullptr;


	EditBase  * actEditMethod    = nullptr;
	EditSpline* editMethodSpline = nullptr;
	EditPen   * editMethodPen    = nullptr;

	ThicknessmapConfig thicknessmapConfig;
// 	Colormap* thicknessmapColor = nullptr;

	bool showSegmentationlines = true;
	bool showThicknessmap      = true;
	bool changeActBScan        = false;

	cv::Mat* thicknesMapImage = nullptr;

	void copySegLinesFromOctDataWhenNotFilled();
	void copySegLinesFromOctDataWhenNotFilled(std::size_t bscan);

	void copySegLinesFromOctData();
	void copySegLinesFromOctData(std::size_t bscan);

	std::size_t getMaxBscanWidth() const;

	void rangeModified(std::size_t ascanBegin, std::size_t ascanEnd);
	void modifiedSegPart(std::size_t bscan, OctData::Segmentationlines::SegmentlineType segLine, std::size_t start, const std::vector<double>& segPart, bool updateMethode);
	void updateEditLine();

	std::vector<double> getSegPart(const std::vector<double>& segLine, std::size_t ascanBegin, std::size_t ascanEnd);
signals:
	void segMethodChanged();
	void segLineIdChanged(std::size_t id);
	void segLineVisibleChanged(bool);

public slots:
	void setSegmentationLinesVisible(bool visible);
	void setThicknessmapVisible(bool visible);

	void generateThicknessmap();

	void setActEditLinetype(OctData::Segmentationlines::SegmentlineType type);
	void highlightLinetype (OctData::Segmentationlines::SegmentlineType type);
	void highlightNoLinetype();
};

#endif // BSCANLAYERSEGMENTATION_H
