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

	localOpPaint              = new BScanSegLocalOpPaint    (*this);
	localOpThresholdDirection = new BScanSegLocalOpThresholdDirection(*this);
	localOpThreshold          = new BScanSegLocalOpThreshold(*this);
	localOpOperation          = new BScanSegLocalOpOperation(*this);

	setLocalMethod(BScanSegmentationMarker::LocalMethod::Paint);

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
		inline static void paint(QPainter& painter, uint8_t* p00, uint8_t* p10, uint8_t* p01, int w, int h, int /*factor*/)
		{
			if(*p00 != *p10)
			{
				painter.drawPoint(w, h);
				painter.drawPoint(w+1, h);
			}
			if(*p00 != *p01)
			{
				painter.drawPoint(w, h);
				painter.drawPoint(w, h+1);
			}
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
void BScanSegmentation::drawSegmentLine(QPainter& painter, double factor, const QRect& rect) const
{
	cv::Mat* map = segments.at(getActBScanNr());
	if(!map || map->empty())
		return;

	if(factor <= 0)
		return;

	int drawX      = static_cast<int>((rect.x()     )/factor + 0.5)-2;
	int drawY      = static_cast<int>((rect.y()     )/factor + 0.5)-2;
	int drawWidth  = static_cast<int>((rect.width() )/factor + 0.5)+4;
	int drawHeight = static_cast<int>((rect.height())/factor + 0.5)+4;

	QPen pen(Qt::red);
	pen.setWidth(seglinePaintSize);
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
			T::paint(painter, p00, p10, p01, w, h, static_cast<int>(factor)); // for faster drawing, only int supported (and only int used at the moment)

			++p00;
			++p10;
			++p01;
		}
	}
}

void BScanSegmentation::transformCoordWidget2Mat(int xWidget, int yWidget, double factor, int& xMat, int& yMat)
{
	yMat = static_cast<int>(yWidget/factor + 0.5);
	xMat = static_cast<int>(xWidget/factor + 0.5);
}




void BScanSegmentation::drawMarker(QPainter& p, BScanMarkerWidget* widget, const QRect& rect) const
{
	double factor = widget->getImageScaleFactor();
	if(factor <= 0)
		return;
	
	if(factor == 1)
		drawSegmentLine<PaintFactor1>(p, factor, rect);
	else
		drawSegmentLine<PaintFactorN>(p, factor, rect);

	QPoint paintPoint = mousePoint;
	if(factor > 1)
	{
		int x = static_cast<int>(std::round(static_cast<double>(mousePoint.x())/factor)*factor + 0.5);
		int y = static_cast<int>(std::round(static_cast<double>(mousePoint.y())/factor)*factor + 0.5);
		paintPoint = QPoint(x, y);
	}

	if(inWidget && markerActive && actLocalOperator)
	{
		QPen pen(Qt::green);
		p.setPen(pen);
		actLocalOperator->drawMarkerPaint(p, paintPoint, factor);
	}
}


bool BScanSegmentation::setOnCoord(int x, int y, double factor)
{
	if(factor == 0)
		return false;

	int xD, yD;
	transformCoordWidget2Mat(x, y, factor, xD, yD);

	if(actLocalOperator)
		return actLocalOperator->drawOnCoord(xD, yD);

	return false;
}



bool BScanSegmentation::startOnCoord(int x, int y, double factor)
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


QRect BScanSegmentation::getWidgetPaintSize(const QPoint& p1, const QPoint& p2, double factor)
{
	int height = 2;
	int width  = 2;
	if(actLocalOperator)
	{
		height += actLocalOperator->getOperatorHeight();
		width  += actLocalOperator->getOperatorWidth ();
	}

	height = static_cast<int>(height*factor + 0.5);
	width  = static_cast<int>(width *factor + 0.5);

	QRect rect = QRect(p1, p2).normalized(); // old and new pos
	rect.adjust(-width, -height, width, height);

	return rect;
}


BscanMarkerBase::RedrawRequest BScanSegmentation::mouseMoveEvent(QMouseEvent* e, BScanMarkerWidget* widget)
{
	RedrawRequest result;

	inWidget = true;

	if(markerActive)
	{
		if(!(e->buttons() & Qt::LeftButton))
			paint = false;

		double factor = widget->getImageScaleFactor();

		result.redraw = false; // cursor need redraw
		result.rect   = getWidgetPaintSize(mousePoint, e->pos(), factor);

		int x = e->x();
		int y = e->y();

		if(actLocalOperator)
		{
			if(paint)
				result.redraw = setOnCoord(x, y, factor);

			result.redraw |= actLocalOperator->drawMarker();
		}
	}
	mousePoint = e->pos();

	return result;
}

BscanMarkerBase::RedrawRequest  BScanSegmentation::mousePressEvent(QMouseEvent* e, BScanMarkerWidget* widget)
{
	double factor = widget->getImageScaleFactor();

	RedrawRequest result;
	result.redraw = false;
	result.rect   = getWidgetPaintSize(mousePoint, e->pos(), factor);

	paint = (e->buttons() == Qt::LeftButton);
	if(paint)
	{
		startOnCoord(e->x(), e->y(), factor);
		// if(autoPaintValue)
		//	paintValue = valueOnCoord(e->x(), e->y(), widget->getImageScaleFactor());
		
		result.redraw = setOnCoord(e->x(), e->y(), factor);
	}
	return result;
}

BscanMarkerBase::RedrawRequest  BScanSegmentation::mouseReleaseEvent(QMouseEvent* e, BScanMarkerWidget* widget)
{
	double factor = widget->getImageScaleFactor();

	RedrawRequest result;
	result.redraw = false;
	result.rect   = getWidgetPaintSize(mousePoint, e->pos(), factor);

	paint = false;
	if(factor == 0)
		return result;

	if(actLocalOperator)
	{
		int x = e->x();
		int y = e->y();

		int xD, yD;
		transformCoordWidget2Mat(x, y, factor, xD, yD);

		result.redraw = actLocalOperator->endOnCoord(xD, yD);
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
	switch(e->key())
	{
		case Qt::Key_X:
			opencloseBScan();
			return true;
		case Qt::Key_V:
			medianBScan();
			return true;
		case Qt::Key_1:
			setLocalMethod(BScanSegmentationMarker::LocalMethod::ThresholdDirection);
			return true;
		case Qt::Key_2:
			setLocalMethod(BScanSegmentationMarker::LocalMethod::Paint);
			return true;
		case Qt::Key_3:
			setLocalMethod(BScanSegmentationMarker::LocalMethod::Operation);
			return true;
		case Qt::Key_4:
			setLocalMethod(BScanSegmentationMarker::LocalMethod::Threshold);
			return true;
	}

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

	BScanSegAlgorithm::openClose(*map);

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

void BScanSegmentation::createSegments()
{
	const OctData::Series* series = getSeries();

	if(!series)
		return;

	clearSegments();

	for(const OctData::BScan* bscan : series->getBScans())
	{
		cv::Mat* mat = new cv::Mat(bscan->getHeight(), bscan->getWidth(), cv::DataType<uint8_t>::type, cvScalar(BScanSegmentationMarker::markermatInitialValue));
		segments.push_back(mat);
	}
}


void BScanSegmentation::newSeriesLoaded(const OctData::Series* series, boost::property_tree::ptree& markerTree)
{
	if(!series)
		return;

	clearSegments();

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


void BScanSegmentation::initBScanFromThreshold(const BScanSegmentationMarker::ThresholdDirectionData& data)
{
	cv::Mat* map = segments.at(getActBScanNr());
	if(!map || map->empty())
		return;

	const OctData::Series* series = getSeries();
	if(!series)
		return;

	const OctData::BScan* bscan = series->getBScan(getActBScanNr());
	if(!bscan)
		return;

	const cv::Mat& image = bscan->getImage();
	if(image.empty())
		return;


	BScanSegAlgorithm::initFromThresholdDirection(image, *map, data);

	requestUpdate();
}


void BScanSegmentation::initSeriesFromThreshold(const BScanSegmentationMarker::ThresholdDirectionData& data)
{
	const OctData::Series* series = getSeries();
	SegMats::iterator segMatIt = segments.begin();


	for(const OctData::BScan* bscan : series->getBScans())
	{
		const cv::Mat& image = bscan->getImage();
		cv::Mat* mat = *segMatIt;

		if(mat && !mat->empty())
			BScanSegAlgorithm::initFromThresholdDirection(image, *mat, data);

		++segMatIt;
	}
	requestUpdate();
}

void BScanSegmentation::initBScanFromSegline()
{
	cv::Mat* map = segments.at(getActBScanNr());
	if(!map || map->empty())
		return;

	const OctData::Series* series = getSeries();
	if(!series)
		return;

	const OctData::BScan* bscan = series->getBScan(getActBScanNr());
	if(!bscan)
		return;

	BScanSegAlgorithm::initFromSegline(*bscan, *map);

	requestUpdate();
}

void BScanSegmentation::initSeriesFromSegline()
{
	const OctData::Series* series = getSeries();
	SegMats::iterator segMatIt = segments.begin();


	for(const OctData::BScan* bscan : series->getBScans())
	{
		if(!bscan)
			continue;
		cv::Mat* mat = *segMatIt;

		if(mat && !mat->empty())
			BScanSegAlgorithm::initFromSegline(*bscan, *mat);

		++segMatIt;
	}
	requestUpdate();
}



void BScanSegmentation::setLocalMethod(BScanSegmentationMarker::LocalMethod method)
{
	if(localMethod != method)
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
			case BScanSegmentationMarker::LocalMethod::ThresholdDirection:
				actLocalOperator = localOpThresholdDirection;
				break;
			case BScanSegmentationMarker::LocalMethod::None:
				actLocalOperator = nullptr;
				break;
		}
		localOperatorChanged(method);
	}
}

void BScanSegmentation::setSeglinePaintSize(int size)
{
	seglinePaintSize = size;
	requestUpdate();
}


