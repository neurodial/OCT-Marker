#include "bscanlayersegmentation.h"

#include <widgets/bscanmarkerwidget.h>
#include <QPainter>
#include <QMouseEvent>

#include <octdata/datastruct/series.h>
#include <octdata/datastruct/bscan.h>
#include <data_structure/programoptions.h>

#include "wglayerseg.h"


#include"editbase.h"
#include"editpen.h"
#include"editspline.h"

#include"bscanlayersegptree.h"
#include <manager/octmarkermanager.h>

#include <cpp_framework/cvmat/cvmattreestruct.h>
#include <cpp_framework/cvmat/treestructbin.h>
#include "layersegmentationio.h"


BScanLayerSegmentation::BScanLayerSegmentation(OctMarkerManager* markerManager)
: BscanMarkerBase(markerManager)
, editMethodSpline(new EditSpline(this))
, editMethodPen   (new EditPen   (this))
{
	name = tr("Layer Segmentation");
	id   = "LayerSegmentation";
	icon = QIcon(":/icons/typicons_mod/layer_seg.svg");

	widgetPtr2WGLayerSeg = new WGLayerSeg(this);

	actEditMethod = editMethodPen;
}

BScanLayerSegmentation::~BScanLayerSegmentation()
{
	delete editMethodSpline;
	delete editMethodPen   ;
}



void BScanLayerSegmentation::drawMarker(QPainter& painter, BScanMarkerWidget* widget, const QRect& rec) const
{
	int bScanHeight = getBScanHight();
	const ScaleFactor& scaleFactor = widget->getImageScaleFactor();

	QPen penEdit;
	penEdit.setColor(ProgramOptions::layerSegActiveLineColor());
	penEdit.setWidth(ProgramOptions::layerSegActiveLineSize ());

	QPen penNormal;
	penNormal.setColor(ProgramOptions::layerSegPassivLineColor());
	penNormal.setWidth(ProgramOptions::layerSegPassivLineSize ());


	for(OctData::Segmentationlines::SegmentlineType type : OctData::Segmentationlines::getSegmentlineTypes())
	{
		if(type == actEditType)
			painter.setPen(penEdit);
		else
			painter.setPen(penNormal);

		BScanMarkerWidget::paintSegmentationLine(painter, bScanHeight, lines[getActBScanNr()].lines.getSegmentLine(type), scaleFactor);
	}

	if(actEditMethod)
		actEditMethod->drawMarker(painter, widget, rec, scaleFactor);
}



BscanMarkerBase::RedrawRequest BScanLayerSegmentation::mouseMoveEvent(QMouseEvent* event, BScanMarkerWidget* widget)
{
	if(actEditMethod)
		return actEditMethod->mouseMoveEvent(event, widget);
	return BscanMarkerBase::RedrawRequest();
}

BscanMarkerBase::RedrawRequest BScanLayerSegmentation::mousePressEvent(QMouseEvent* event, BScanMarkerWidget* widget)
{
// 	paintSegLine = true;
// 	double scaleFactor = widget->getImageScaleFactor();
// 	lastPoint = calcPoint(event->x(), event->y(), scaleFactor, getBScanWidth());
	if(actEditMethod)
		return actEditMethod->mousePressEvent(event, widget);
	return BscanMarkerBase::RedrawRequest();
}

BscanMarkerBase::RedrawRequest BScanLayerSegmentation::mouseReleaseEvent(QMouseEvent* event, BScanMarkerWidget* widget)
{
	if(actEditMethod)
		return actEditMethod->mouseReleaseEvent(event, widget);
// 	paintSegLine = false;
	return BscanMarkerBase::RedrawRequest();
}

void BScanLayerSegmentation::contextMenuEvent(QContextMenuEvent* event)
{
	if(actEditMethod)
		actEditMethod->contextMenuEvent(event);
}


void BScanLayerSegmentation::newSeriesLoaded(const OctData::Series* series, boost::property_tree::ptree& ptree)
{
	resetMarkers(series);
	loadState(ptree);
}


void BScanLayerSegmentation::resetMarkers(const OctData::Series* series)
{
	if(!series)
		return;

	const std::size_t numBscans = series->bscanCount();

	lines.clear();
	lines.resize(numBscans);

	for(std::size_t i = 0; i<numBscans; ++i)
	{
		const int bscanWidth = series->getBScan(i)->getWidth();

		for(OctData::Segmentationlines::SegmentlineType type : OctData::Segmentationlines::getSegmentlineTypes())
			lines[i].lines.getSegmentLine(type).resize(bscanWidth);
	}

}


void BScanLayerSegmentation::setActEditLinetype(OctData::Segmentationlines::SegmentlineType type)
{
	if(type == actEditType)
		return;

	actEditType = type;


	if(actEditMethod)
		actEditMethod->segLineChanged(&lines[getActBScanNr()].lines.getSegmentLine(actEditType));

	requestFullUpdate();
}


bool BScanLayerSegmentation::keyPressEvent(QKeyEvent* event, BScanMarkerWidget* widget)
{
	int key = event->key();
	switch(key)
	{
		case Qt::Key_C:
			copySegLinesFromOctData();
			return true;

		case Qt::Key_1:
			setSegMethod(BScanLayerSegmentation::SegMethod::Pen);
			return true;

		case Qt::Key_2:
			setSegMethod(BScanLayerSegmentation::SegMethod::Spline);
			return true;
	}

	if(actEditMethod)
		return actEditMethod->keyPressEvent(event, widget);

	return false;
}


void BScanLayerSegmentation::copyAllSegLinesFromOctData()
{
	for(std::size_t i = 0; i<lines.size(); ++i)
		copySegLinesFromOctData(i);
}


void BScanLayerSegmentation::copySegLinesFromOctData() { copySegLinesFromOctData(getActBScanNr()); }

void BScanLayerSegmentation::copySegLinesFromOctData(const std::size_t bScanNr)
{
	if(actEditMethod)
		actEditMethod->segLineChanged(nullptr);

	const OctData::BScan* bscan = getBScan(bScanNr);
	if(!bscan)
		return;


	BScanSegData& segData = lines[bScanNr];
	segData.lines  = bscan->getSegmentLines();
	segData.filled = true;

	const std::size_t bscanWidth = static_cast<std::size_t>(bscan->getWidth());
	for(OctData::Segmentationlines::SegmentlineType type : OctData::Segmentationlines::getSegmentlineTypes())
		segData.lines.getSegmentLine(type).resize(bscanWidth);

	if(actEditMethod)
		actEditMethod->segLineChanged(&segData.lines.getSegmentLine(actEditType));

	if(bScanNr == getActBScanNr())
		requestFullUpdate();
}

void BScanLayerSegmentation::setSegMethod(BScanLayerSegmentation::SegMethod method)
{
	if(getSegMethod() == method)
		return;

	if(actEditMethod)
		actEditMethod->segLineChanged(nullptr);

	switch(method)
	{
		case BScanLayerSegmentation::SegMethod::None:
			actEditMethod = nullptr;
			break;
		case BScanLayerSegmentation::SegMethod::Pen:
			actEditMethod = editMethodPen;
			break;
		case BScanLayerSegmentation::SegMethod::Spline:
			actEditMethod = editMethodSpline;
			break;
	}

	if(actEditMethod)
		actEditMethod->segLineChanged(&lines[getActBScanNr()].lines.getSegmentLine(actEditType));

	emit(segMethodChanged());

	requestFullUpdate();
}


BScanLayerSegmentation::SegMethod BScanLayerSegmentation::getSegMethod() const
{
	if(actEditMethod == editMethodPen   ) return BScanLayerSegmentation::SegMethod::Pen;
	if(actEditMethod == editMethodSpline) return BScanLayerSegmentation::SegMethod::Spline;

	return BScanLayerSegmentation::SegMethod::None;
}


void BScanLayerSegmentation::copySegLinesFromOctDataWhenNotFilled() { copySegLinesFromOctDataWhenNotFilled(getActBScanNr()); }

void BScanLayerSegmentation::copySegLinesFromOctDataWhenNotFilled(std::size_t bscan)
{
	if(bscan < lines.size())
	{
		BScanSegData& segData = lines[bscan];
		if(!segData.filled)
			copySegLinesFromOctData(bscan);
	}
}



void BScanLayerSegmentation::setActBScan(std::size_t bscan)
{
	if(isActivated)
		copySegLinesFromOctDataWhenNotFilled(bscan);

	BScanSegData& segData = lines[bscan];
	if(actEditMethod)
		actEditMethod->segLineChanged(&segData.lines.getSegmentLine(actEditType));
}

void BScanLayerSegmentation::loadState(boost::property_tree::ptree& markerTree)
{
	BScanLayerSegPTree::parsePTree(markerTree, this);
}

void BScanLayerSegmentation::saveState(boost::property_tree::ptree& markerTree)
{
	BScanLayerSegPTree::fillPTree(markerTree, this);
}

bool BScanLayerSegmentation::saveSegmentation2Bin(const std::string& filename)
{
	return LayerSegmentationIO::saveSegmentation2Bin(*this, filename);
}



std::size_t BScanLayerSegmentation::getMaxBscanWidth() const // TODO: Codedopplung mit IntervalMarker
{
	const OctData::Series* series = getSeries();
	if(!series)
		return 0;

	std::size_t maxBscanWidth = 0;
	for(const OctData::BScan* bscan : series->getBScans())
	{
		if(bscan)
		{
			if(maxBscanWidth < bscan->getWidth())
				maxBscanWidth = static_cast<std::size_t>(bscan->getWidth());
		}
	}
	return maxBscanWidth;
}

void BScanLayerSegmentation::setIconsToSimple(int size)
{
	WGLayerSeg* widget = dynamic_cast<WGLayerSeg*>(widgetPtr2WGLayerSeg);
	if(widget)
		widget->setIconsToSimple(size);
}

