#include "bscansegmentation.h"

#include <QPainter>
#include <QMouseEvent>
// #include <QIcon>
// #include <QActionGroup>
// #include <QAction>
#include <QWidget>

#include <manager/bscanmarkermanager.h>

#include <widgets/bscanmarkerwidget.h>

#include <octdata/datastruct/bscan.h>
#include <octdata/datastruct/series.h>

#include <opencv/cv.h>

#include "bscansegmentationptree.h"

#include "wgsegmentation.h"
#include "bscansegalgorithm.h"
#include "bscansegtoolbar.h"

#include "bscanseglocalop.h"


BScanSegmentation::BScanSegmentation(BScanMarkerManager* markerManager)
: BscanMarkerBase(markerManager)
{
	name = tr("Segmentation marker");
	id   = "SegmentationMarker";
	
	icon = QIcon(":/icons/segline_edit.png");

	localOpPaint     = new BScanSegLocalOpPaint    (*this);
	localOpThreshold = new BScanSegLocalOpThreshold(*this);
	localOpOperation = new BScanSegLocalOpOperation(*this);

	setLocalMethod(localMethod);

	widget = new WGSegmentation(this);
	widgetPtr2WGSegmentation = widget;

	
	// connect(markerManager, &BScanMarkerManager::newSeriesShowed, this, &BScanSegmentation::newSeriesLoaded);
}

BScanSegmentation::~BScanSegmentation()
{
	clearSegments();

	delete widget;
}


QWidget* BScanSegmentation::createWidget(QWidget* parent)
{
	return new QWidget(parent);;
}

QToolBar* BScanSegmentation::createToolbar(QObject* parent)
{
	BScanSegToolBar* toolBar = new BScanSegToolBar(this, parent);
	connectToolBar(toolBar);

	return toolBar;
}



// ----------------------
// Draw segmentation line
// ----------------------

namespace
{
	struct PaintFactor1
	{
		inline static void paint(QPainter& painter, uint8_t* p00, uint8_t* p10, uint8_t* p01, int w, int h, int factor)
		{
			if(*p00 != *p10)
				painter.drawPoint(w*factor, h*factor);
			else if(*p00 != *p01)
				painter.drawPoint(w*factor, h*factor);
		}
	};

	struct PaintFactorN
	{
		inline static void paint(QPainter& painter, uint8_t* p00, uint8_t* p10, uint8_t* p01, int w, int h, int factor)
		{
			if(*p00 != *p10)
				painter.drawLine((w+1)*factor, (h)*factor, (w+1)*factor, (h+1)*factor);
			if(*p00 != *p01)
				painter.drawLine((w)*factor, (h+1)*factor, (w+1)*factor, (h+1)*factor);
		}
	};
}


template<typename T>
void BScanSegmentation::drawSegmentLine(QPainter& painter, int factor, const QRect& rect) const
{
	cv::Mat* map = segments.at(getActBScanNr());
	if(!map || map->empty())
		return;

	if(factor <= 0)
		return;

	int drawX      = (rect.x()     )/factor-2;
	int drawY      = (rect.y()     )/factor-2;
	int drawWidth  = (rect.width() )/factor+4;
	int drawHeight = (rect.height())/factor+4;

	QPen pen(Qt::red);
	painter.setPen(pen);

	int mapHeight = map->rows-1; // -1 for p01
	int mapWidth  = map->cols-1; // -1 for p10

	int startH = std::max(drawY, 0);
	int endH   = std::min(drawY+drawHeight, mapHeight);
	int startW = std::max(drawX, 0);
	int endW   = std::min(drawX+drawWidth , mapWidth);

	for(int h = startH; h < endH; ++h)
	{
		uint8_t* p00 = map->ptr<uint8_t>(h);
		uint8_t* p10 = p00+1;
		uint8_t* p01 = map->ptr<uint8_t>(h+1);

		p00 += startW;
		p10 += startW;
		p01 += startW;

		for(int w = startW; w < endW; ++w)
		{
			T::paint(painter, p00, p10, p01, w, h, factor);

			++p00;
			++p10;
			++p01;
		}
	}
}

void BScanSegmentation::transformCoordWidget2Mat(int xWidget, int yWidget, int factor, int& xMat, int& yMat)
{
	double factorD = factor;
	xMat = static_cast<int>(std::round(xWidget/factorD));
	yMat = static_cast<int>(std::round(yWidget/factorD));
}




void BScanSegmentation::drawMarker(QPainter& p, BScanMarkerWidget* widget, const QRect& rect) const
{
	int factor = widget->getImageScaleFactor();
	if(factor <= 0)
		return;
	
	if(factor == 1)
		drawSegmentLine<PaintFactor1>(p, factor, rect);
	else
		drawSegmentLine<PaintFactorN>(p, factor, rect);

	QPoint paintPoint = mousePoint;
	if(factor > 1)
	{
		int x = std::round(static_cast<double>(mousePoint.x())/factor)*factor;
		int y = std::round(static_cast<double>(mousePoint.y())/factor)*factor;
		paintPoint = QPoint(x, y);
	}

	QPen pen(Qt::green);
	p.setPen(pen);
	if(inWidget && actLocalOperator)
		actLocalOperator->drawMarkerPaint(p, paintPoint, factor);
}


bool BScanSegmentation::setOnCoord(int x, int y, int factor)
{
	if(factor == 0)
		return false;

	int xD, yD;
	transformCoordWidget2Mat(x, y, factor, xD, yD);

	if(actLocalOperator)
		actLocalOperator->drawOnCoord(xD, yD);

	return true;
}



bool BScanSegmentation::startOnCoord(int x, int y, int factor)
{
	if(factor == 0)
		return false;

	int xD, yD;
	transformCoordWidget2Mat(x, y, factor, xD, yD);

	if(actLocalOperator)
		actLocalOperator->startOnCoord(xD, yD);

	return true;
}




uint8_t BScanSegmentation::valueOnCoord(int x, int y)
{
	cv::Mat* map = segments.at(getActBScanNr());
	if(!map || map->empty())
		return BScanSegmentationMarker::markermatInitialValue;

	return map->at<uint8_t>(cv::Point(x, y));
}


QRect BScanSegmentation::getWidgetPaintSize(const QPoint& p1, const QPoint& p2, int factor)
{
	int height = 2;
	int width  = 2;
	if(actLocalOperator)
	{
		height += actLocalOperator->getOperatorHeight();
		width  += actLocalOperator->getOperatorWidth ();
	}

	height *= factor;
	width  *= factor;

	QRect rect = QRect(p1, p2).normalized(); // old and new pos
	rect.adjust(-width, -height, width, height);

	return rect;
}


BscanMarkerBase::RedrawRequest BScanSegmentation::mouseMoveEvent(QMouseEvent* e, BScanMarkerWidget* widget)
{
	RedrawRequest result;

	if(!(e->buttons() & Qt::LeftButton))
		paint = false;
		
	inWidget = true;
	int factor = widget->getImageScaleFactor();

	result.redraw = true;
	result.rect   = getWidgetPaintSize(mousePoint, e->pos(), factor);

	mousePoint = e->pos();
	int x = e->x();
	int y = e->y();
	
	if(paint)
		setOnCoord(x, y, factor);

	return result;
}

bool BScanSegmentation::mousePressEvent(QMouseEvent* e, BScanMarkerWidget* widget)
{
	paint = (e->buttons() == Qt::LeftButton);
	if(paint)
	{
		startOnCoord(e->x(), e->y(), widget->getImageScaleFactor());
		// if(autoPaintValue)
		//	paintValue = valueOnCoord(e->x(), e->y(), widget->getImageScaleFactor());
		
		return setOnCoord(e->x(), e->y(), widget->getImageScaleFactor());
	}
	return false;
}

bool BScanSegmentation::mouseReleaseEvent(QMouseEvent* e, BScanMarkerWidget* widget)
{
	bool result = false;
	paint = false;

	if(actLocalOperator)
	{
		int factor = widget->getImageScaleFactor();
		int x = e->x();
		int y = e->y();
		if(factor == 0)
			return false;

		int xD, yD;
		transformCoordWidget2Mat(x, y, factor, xD, yD);

		result = actLocalOperator->endOnCoord(xD, yD);
	}

	return result;
}




bool BScanSegmentation::leaveWidgetEvent(QEvent*, BScanMarkerWidget*)
{
	inWidget = false;
	return true;
}


bool BScanSegmentation::keyPressEvent(QKeyEvent* e, BScanMarkerWidget*)
{
	/*
	switch(e->key())
	{
		case Qt::Key_1:
			paintArea0Slot();
			return true;
		case Qt::Key_2:
			autoAddRemoveArea();
			return true;
		case Qt::Key_3:
			paintArea1Slot();
			return true;
	}
	*/

	return false;
}





void BScanSegmentation::dilateBScan()
{
	cv::Mat* map = segments.at(getActBScanNr());
	if(!map || map->empty())
		return;

	int iterations = 1;
	cv::dilate(*map, *map, cv::Mat(), cv::Point(-1, -1), iterations, cv::BORDER_REFLECT_101, 1);

	requestUpdate();
}

void BScanSegmentation::erodeBScan()
{
	cv::Mat* map = segments.at(getActBScanNr());
	if(!map || map->empty())
		return;

	int iterations = 1;
	cv::erode(*map, *map, cv::Mat(), cv::Point(-1, -1), iterations, cv::BORDER_REFLECT_101, 1);

	requestUpdate();
}

void BScanSegmentation::opencloseBScan()
{
	cv::Mat* map = segments.at(getActBScanNr());
	if(!map || map->empty())
		return;

	int iterations = 1;
	cv::erode (*map, *map, cv::Mat(), cv::Point(-1, -1), iterations  , cv::BORDER_REFLECT_101, 1);
	cv::dilate(*map, *map, cv::Mat(), cv::Point(-1, -1), iterations*2, cv::BORDER_REFLECT_101, 1);
	cv::erode (*map, *map, cv::Mat(), cv::Point(-1, -1), iterations  , cv::BORDER_REFLECT_101, 1);

	requestUpdate();
}


void BScanSegmentation::medianBScan()
{
	cv::Mat* map = segments.at(getActBScanNr());
	if(!map || map->empty())
		return;

	medianBlur(*map, *map, 3);

	requestUpdate();
}



void BScanSegmentation::clearSegments()
{
	for(cv::Mat* mat : segments)
		delete mat;

	segments.clear();
}

void BScanSegmentation::newSeriesLoaded(const OctData::Series* series, boost::property_tree::ptree& markerTree)
{
	clearSegments();

	if(!series)
		return;

	for(const OctData::BScan* bscan : series->getBScans())
	{
		cv::Mat* mat = new cv::Mat(bscan->getHeight(), bscan->getWidth(), cv::DataType<uint8_t>::type, cvScalar(BScanSegmentationMarker::markermatInitialValue));
		segments.push_back(mat);
	}

	BScanSegmentationPtree::parsePTree(markerTree, this);
}

void BScanSegmentation::saveState(boost::property_tree::ptree& markerTree)
{
	BScanSegmentationPtree::fillPTree(markerTree, this);
}

void BScanSegmentation::loadState(boost::property_tree::ptree& markerTree)
{
	for(cv::Mat* mat : segments)
	{
		if(mat)
		{
			mat->setTo(cv::Scalar(BScanSegmentationMarker::markermatInitialValue));
		}
	}

	BScanSegmentationPtree::parsePTree(markerTree, this);
}


void BScanSegmentation::updateCursor()
{
	if(inWidget)
		requestUpdate();
}


void BScanSegmentation::initBScanFromThreshold(const BScanSegmentationMarker::ThresholdData& data)
{
	cv::Mat* map = segments.at(getActBScanNr());
	if(!map || map->empty())
		return;

	const OctData::Series* series = getSeries();
	if(!series)
		return;

	const OctData::BScan* bscan = series->getBScan(getActBScanNr());

	const cv::Mat& image = bscan->getImage();
	if(image.empty())
		return;


	BScanSegAlgorithm::initFromThreshold(image, *map, data);

	requestUpdate();
}


void BScanSegmentation::initSeriesFromThreshold(const BScanSegmentationMarker::ThresholdData& data)
{
	const OctData::Series* series = getSeries();
	SegMats::iterator segMatIt = segments.begin();


	for(const OctData::BScan* bscan : series->getBScans())
	{
		const cv::Mat& image = bscan->getImage();
		cv::Mat* mat = *segMatIt;

		if(mat && !mat->empty())
			BScanSegAlgorithm::initFromThreshold(image, *mat, data);

		++segMatIt;
	}
	requestUpdate();
}


void BScanSegmentation::setLocalMethod(BScanSegmentationMarker::LocalMethod method)
{
	localMethod = method;
	switch(localMethod)
	{
		case BScanSegmentationMarker::LocalMethod::Paint:
			actLocalOperator = localOpPaint;
			break;
		case BScanSegmentationMarker::LocalMethod::Operation:
			actLocalOperator = localOpOperation;
			break;
		case BScanSegmentationMarker::LocalMethod::Threshold:
			actLocalOperator = localOpThreshold;
			break;
	}
}

