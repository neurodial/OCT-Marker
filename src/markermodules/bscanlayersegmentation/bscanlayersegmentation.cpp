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

BScanLayerSegmentation::BScanLayerSegmentation(OctMarkerManager* markerManager)
: BscanMarkerBase(markerManager)
, editMethodSpline(new EditSpline(this))
, editMethodPen(new EditPen(this))
{
	name = tr("Layer Segmentation");
	id   = "layerSegmentation";
	icon = QIcon(":/icons/seglinelayer_edit.png");

	widgetPtr2WGLayerSeg = new WGLayerSeg(this);

	actEditMethod = editMethodPen;
}

BScanLayerSegmentation::~BScanLayerSegmentation()
{
	delete editMethodSpline;
	delete editMethodPen;
}


namespace
{

	void paintPolygon(QPainter& painter, const std::vector<Point2D>& polygon, double factor)
	{
		if(polygon.size() < 2)
			return;

		      std::vector<Point2D>::const_iterator it    = polygon.begin();
		const std::vector<Point2D>::const_iterator itEnd = polygon.end();
		Point2D lastPoint = *it;
		++it;
		while(it != itEnd)
		{
			painter.drawEllipse(lastPoint.getX()*factor-4, lastPoint.getY()*factor-4, 8, 8);
// 			painter.drawLine(lastPoint.getX()*factor, lastPoint.getY()*factor, it->getX()*factor, it->getY()*factor);
			lastPoint = *it;
			++it;
		}
		painter.drawEllipse(lastPoint.getX()*factor, lastPoint.getY()*factor, 5, 5);
	}

}




void BScanLayerSegmentation::drawMarker(QPainter& painter, BScanMarkerWidget* widget, const QRect& rec) const
{
	int bScanHeight = getBScanHight();
	double scaleFactor = widget->getImageScaleFactor();


	QPen penEdit;
	penEdit.setColor(Qt::red);
	penEdit.setWidth(2);

	QPen penNormal;
	penNormal.setColor(Qt::blue);
	penNormal.setWidth(1);


	for(OctData::Segmentationlines::SegmentlineType type : OctData::Segmentationlines::getSegmentlineTypes())
	{
		if(type == actEditType)
			painter.setPen(penEdit);
		else
			painter.setPen(penNormal);

		BScanMarkerWidget::paintSegmentationLine(painter, bScanHeight, lines[getActBScanNr()].getSegmentLine(type), scaleFactor);
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

void BScanLayerSegmentation::newSeriesLoaded(const OctData::Series* series, boost::property_tree::ptree& ptree)
{
	resetMarkers(series);

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
			lines[i].getSegmentLine(type).resize(bscanWidth);
	}

}


void BScanLayerSegmentation::setActEditLinetype(OctData::Segmentationlines::SegmentlineType type)
{
	if(type == actEditType)
		return;

	actEditType = type;


	if(actEditMethod)
		actEditMethod->segLineChanged(&lines[getActBScanNr()].getSegmentLine(actEditType));

	requestFullUpdate();
}


bool BScanLayerSegmentation::keyPressEvent(QKeyEvent* event, BScanMarkerWidget*)
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

	return false;
}

void BScanLayerSegmentation::copySegLinesFromOctData()
{
	if(actEditMethod)
		actEditMethod->segLineChanged(nullptr);

	const OctData::BScan* bscan = getActBScan();
	if(!bscan)
		return;

	const std::size_t actBScanNr = getActBScanNr();

	lines[actBScanNr] = bscan->getSegmentLines();

	const int bscanWidth = bscan->getWidth();
	for(OctData::Segmentationlines::SegmentlineType type : OctData::Segmentationlines::getSegmentlineTypes())
		lines[actBScanNr].getSegmentLine(type).resize(bscanWidth);

	if(actEditMethod)
		actEditMethod->segLineChanged(&lines[getActBScanNr()].getSegmentLine(actEditType));

	requestFullUpdate();
}

void BScanLayerSegmentation::setSegMethod(BScanLayerSegmentation::SegMethod method)
{
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
	{
		actEditMethod->segLineChanged(&lines[getActBScanNr()].getSegmentLine(actEditType));
	}
}

