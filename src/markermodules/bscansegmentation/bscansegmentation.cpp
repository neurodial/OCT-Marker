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


BScanSegmentation::BScanSegmentation(BScanMarkerManager* markerManager)
: BscanMarkerBase(markerManager)
, widget(new WGSegmentation(this))
{
	widgetPtr2WGSegmentation = widget;

	name = tr("Segmentation marker");
	id   = "SegmentationMarker";
	
	icon = QIcon(":/icons/segline_edit.png");
	
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


void BScanSegmentation::drawMarkerOperation(QPainter& painter, const QPoint& centerDrawPoint, int factor) const
{
	int size = localOperatorSize*factor;
	painter.drawRect(centerDrawPoint.x()-size, centerDrawPoint.y()-size, size*2, size*2);
}

void BScanSegmentation::drawMarkerThreshold(QPainter& painter, const QPoint& centerDrawPoint, int factor) const
{
	int size = localOperatorSize*factor;
	painter.drawRect(centerDrawPoint.x()-size, centerDrawPoint.y()-size, size*2, size*2);
}


void BScanSegmentation::drawMarkerPaint(QPainter& painter, const QPoint& centerDrawPoint, int factor) const
{
	switch(localPaintData.paintMethod)
	{
		case BScanSegmentationMarker::PaintData::PaintMethod::Circle:
			painter.drawEllipse(centerDrawPoint + QPoint(factor/2, factor/2), static_cast<int>((localOperatorSize+0.5)*factor), static_cast<int>((localOperatorSize+0.5)*factor));
			break;
		case BScanSegmentationMarker::PaintData::PaintMethod::Rect:
		{
			int size = localOperatorSize*factor;
			painter.drawRect(centerDrawPoint.x()-size, centerDrawPoint.y()-size, size*2, size*2);
			break;
		}
	}
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
	if(inWidget)
	{
		switch(localMethod)
		{
			case BScanSegmentationMarker::LocalMethod::Paint:
				drawMarkerPaint(p, paintPoint, factor);
				break;
			case BScanSegmentationMarker::LocalMethod::Threshold:
				drawMarkerThreshold(p, paintPoint, factor);
				break;
			case BScanSegmentationMarker::LocalMethod::Operation:
				drawMarkerOperation(p, paintPoint, factor);
				break;
		}
	}
}

bool BScanSegmentation::paintOnCoord(cv::Mat* map, int xD, int yD)
{
	if(!map || map->empty())
		return false;

	switch(localPaintData.paintMethod)
	{
		case BScanSegmentationMarker::PaintData::PaintMethod::Circle:
			cv::circle(*map, cv::Point(xD, yD), localOperatorSize, paintValue, CV_FILLED, 8, 0);
			break;
		case BScanSegmentationMarker::PaintData::PaintMethod::Rect:
			cv::rectangle(*map, cv::Point(xD-localOperatorSize, yD-localOperatorSize), cv::Point(xD+localOperatorSize-1, yD+localOperatorSize-1), paintValue, CV_FILLED, 8, 0);
			break;
	}
	return true;
}


bool BScanSegmentation::setOnCoord(int x, int y, int factor)
{
	if(factor == 0)
		return false;

	cv::Mat* map = segments.at(getActBScanNr());
	if(!map || map->empty())
		return false;

	int xD, yD;
	transformCoordWidget2Mat(x, y, factor, xD, yD);

	switch(localMethod)
	{
		case BScanSegmentationMarker::LocalMethod::Paint:
			paintOnCoord(map, xD, yD);
			break;
		case BScanSegmentationMarker::LocalMethod::Threshold:
		case BScanSegmentationMarker::LocalMethod::Operation:
			break;
	}
	return true;
}



bool BScanSegmentation::startOnCoordPaint(int x, int y, int factor)
{
	switch(localPaintData.paintColor)
	{
		case BScanSegmentationMarker::PaintData::PaintColor::Auto:
			paintValue = valueOnCoord(x, y, factor);
			break;
		case BScanSegmentationMarker::PaintData::PaintColor::Area0:
			paintValue = BScanSegmentationMarker::paintArea0Value;
			break;
		case BScanSegmentationMarker::PaintData::PaintColor::Area1:
			paintValue = BScanSegmentationMarker::paintArea1Value;
			break;
	}
	return true;
}


bool BScanSegmentation::startOnCoordOperation(int x, int y, int factor)
{

	cv::Mat* map = segments.at(getActBScanNr());
	if(!map || map->empty())
		return false;

	int dx, dy;
	transformCoordWidget2Mat(x, y, factor, dx, dy);

	int rows = map->rows;
	int cols = map->cols;

	int x0 = std::max(dx - localOperatorSize, 0);
	int x1 = std::min(dx + localOperatorSize, cols-1);
	int y0 = std::max(dy - localOperatorSize, 0);
	int y1 = std::min(dy + localOperatorSize, rows-1);

	if(x0>=x1)
		return false;
	if(y0>=y1)
		return false;

	cv::Mat tmp = (*map)(cv::Rect(x0, y0, x1-x0, y1-y0));

	int iterations = 1;

	switch(localOperation)
	{
		case BScanSegmentationMarker::Operation::Dilate:
			cv::dilate(tmp, tmp, cv::Mat(), cv::Point(-1, -1), iterations, cv::BORDER_REFLECT_101, 1);
			break;
		case BScanSegmentationMarker::Operation::Erode:
			cv::erode(tmp, tmp, cv::Mat(), cv::Point(-1, -1), iterations, cv::BORDER_REFLECT_101, 1);
			break;
	}

	return true;
}


bool BScanSegmentation::startOnCoordThreshold(int x, int y, int factor)
{
	cv::Mat* map = segments.at(getActBScanNr());
	if(!map || map->empty())
		return false;

	int dx, dy;
	transformCoordWidget2Mat(x, y, factor, dx, dy);

	int rows = map->rows;
	int cols = map->cols;

	int x0 = std::max(dx - localOperatorSize, 0);
	int x1 = std::min(dx + localOperatorSize, cols-1);
	int y0 = std::max(dy - localOperatorSize, 0);
	int y1 = std::min(dy + localOperatorSize, rows-1);

	if(x0>=x1)
		return false;
	if(y0>=y1)
		return false;

	const OctData::BScan* bscan = getActBScan();
	if(!bscan)
		return false;

	cv::Mat tmp = (*map)(cv::Rect(x0, y0, x1-x0, y1-y0));
	cv::Mat tmpImages = bscan->getImage()(cv::Rect(x0, y0, x1-x0, y1-y0));

	BScanSegAlgorithm::initFromThreshold(tmpImages, tmp, localThresholdData);

	return true;
}


bool BScanSegmentation::startOnCoord(int x, int y, int factor)
{
	switch(localMethod)
	{
		case BScanSegmentationMarker::LocalMethod::Paint:
			startOnCoordPaint(x, y, factor);
			break;
		case BScanSegmentationMarker::LocalMethod::Threshold:
			startOnCoordThreshold(x, y, factor);
			break;
		case BScanSegmentationMarker::LocalMethod::Operation:
			startOnCoordOperation(x, y, factor);
			break;
	}

	return true;
}




uint8_t BScanSegmentation::valueOnCoord(int x, int y, int factor)
{
	if(factor == 0)
		return 0;
	
	cv::Mat* map = segments.at(getActBScanNr());
	if(!map || map->empty())
		return 0;

	int dx, dy;
	transformCoordWidget2Mat(x, y, factor, dx, dy);
	
	return map->at<uint8_t>(cv::Point(dx, dy));
}


QRect BScanSegmentation::getWidgetPaintSize(const QPoint& p1, const QPoint& p2, int factor)
{
	int drawRad = (localOperatorSize+2)*factor;
	QRect rect = QRect(p1, p2).normalized(); // old and new pos
	rect.adjust(-drawRad, -drawRad, drawRad, drawRad);

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

bool BScanSegmentation::mouseReleaseEvent(QMouseEvent*, BScanMarkerWidget*)
{
	paint = false;
	return false;
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

/*
void BScanSegmentation::setPaintMethodDisc()
{
	paintMethod = PaintMethod::Disc;
	if(inWidget)
		requestUpdate();
}

void BScanSegmentation::setPaintMethodQuadrat()
{
	paintMethod = PaintMethod::Quadrat;
	if(inWidget)
		requestUpdate();
}
*/

void BScanSegmentation::setLocalOperatorSize(int size)
{
	localOperatorSize = size;
	emit(localOperatorSizeChanged(size));

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
}

void BScanSegmentation::setLocalPaintData(const BScanSegmentationMarker::PaintData& data)
{
	localPaintData = data;
}

void BScanSegmentation::setLocalOperation(const BScanSegmentationMarker::Operation& data)
{
	localOperation = data;
}

void BScanSegmentation::setLocalThreshold(const BScanSegmentationMarker::ThresholdData& data)
{
	localThresholdData = data;
}

