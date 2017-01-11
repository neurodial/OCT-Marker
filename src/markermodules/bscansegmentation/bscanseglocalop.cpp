#include "bscanseglocalop.h"

#include "bscansegmentation.h"
#include "bscansegalgorithm.h"

#include <QPainter>
#include <opencv/cv.h>
#include <octdata/datastruct/bscan.h>


cv::Mat* BScanSegLocalOp::getActMat()
{
	return segmentation.actMat;
}

const OctData::BScan* BScanSegLocalOp::getActBScan()
{
	return segmentation.getActBScan();
}

BScanSegmentationMarker::internalMatType BScanSegLocalOp::valueOnCoord(int x, int y)
{
	return segmentation.valueOnCoord(x, y);
}

void BScanSegLocalOp::updateCursor()
{
	segmentation.updateCursor();
}

std::size_t BScanSegLocalOp::getBScanNr()
{
	return segmentation.getActBScanNr();
}




namespace
{
	template<typename T>
	bool assignUpdateNecessary(T& dest, const T& src)
	{
		bool update = true;
		if(dest == src)
			update = false;

		dest = src;
		return update;
	}
}



namespace
{
	QIcon createColorIcon(const QColor& color)
	{
		QPixmap pixmap(15, 15);
		pixmap.fill(color);
		return QIcon(pixmap);
	}

	QIcon createMonocromeIcon(uint8_t grayvalue)
	{
		return createColorIcon(QColor::fromRgb(grayvalue, grayvalue, grayvalue));
	}

}




void BScanSegLocalOpPaint::drawMarkerPaint(QPainter& painter, const QPoint& centerDrawPoint, double factor) const
{
	switch(localPaintData.paintMethod)
	{
		case BScanSegmentationMarker::PaintData::PaintMethod::Circle:
		{
			QPointF point = centerDrawPoint;
			point += QPointF(factor/2, factor/2);
			painter.drawEllipse(point
			                  , (paintSize+0.5)*factor
			                  , (paintSize+0.5)*factor);
			/*
			painter.drawEllipse(centerDrawPoint + QPoint(static_cast<int>(factor/2), static_cast<int>(factor/2))
			                  , (paintSize+0.5)*factor
			                  , (paintSize+0.5)*factor);*/
			break;
		}
		case BScanSegmentationMarker::PaintData::PaintMethod::Rect:
		{
			int size = static_cast<int>(paintSize*factor + 0.5);
			painter.drawRect(centerDrawPoint.x()-size, centerDrawPoint.y()-size, size*2, size*2);
			break;
		}
		case BScanSegmentationMarker::PaintData::PaintMethod::Pen:
			break;
	}
}

bool BScanSegLocalOpPaint::drawOnCoord(int x, int y)
{
	cv::Mat* map = getActMat();
	if(!map || map->empty())
	return false;

	switch(localPaintData.paintMethod)
	{
		case BScanSegmentationMarker::PaintData::PaintMethod::Circle:
			cv::circle(*map, cv::Point(x, y), paintSize, paintValue, CV_FILLED, 8, 0);
			break;
		case BScanSegmentationMarker::PaintData::PaintMethod::Rect:
			cv::rectangle(*map, cv::Point(x-paintSize, y-paintSize), cv::Point(x+paintSize-1, y+paintSize-1), paintValue, CV_FILLED, 8, 0);
			break;
		case BScanSegmentationMarker::PaintData::PaintMethod::Pen:
			if(x>=0 && y>=0 && x<map->cols && y<map->rows)
				map->at<BScanSegmentationMarker::internalMatType>(y, x) = paintValue;
			break;
	}
	return true;
}

bool BScanSegLocalOpPaint::startOnCoord(int x, int y)
{
	switch(localPaintData.paintColor)
	{
		case BScanSegmentationMarker::PaintData::PaintColor::Auto:
			paintValue = valueOnCoord(x, y);
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

void BScanSegLocalOpPaint::setOperatorSize(int size)
{
	size = validOperatorSize(size);
	if(assignUpdateNecessary(paintSize, size))
		updateCursor();
}

void BScanSegLocalOpPaint::setPaintData(const BScanSegmentationMarker::PaintData& data)
{
	if(assignUpdateNecessary(localPaintData, data))
		updateCursor();
}

QIcon BScanSegLocalOpPaint::getPaintColorIcon(BScanSegmentationMarker::PaintData::PaintColor color) const
{
	switch(color)
	{
		case BScanSegmentationMarker::PaintData::PaintColor::Area0:
			return createMonocromeIcon(255);
		case BScanSegmentationMarker::PaintData::PaintColor::Auto:
			return createMonocromeIcon(124);
		case BScanSegmentationMarker::PaintData::PaintColor::Area1:
			return createMonocromeIcon(0);
	}
	return QIcon();
}



// ---------
// Operation
// ---------
void BScanSegLocalOpOperation::drawMarkerPaint(QPainter& painter, const QPoint& centerDrawPoint, double factor) const
{
	int sizeW = static_cast<int>(paintSizeWidth *factor + 0.5);
	int sizeH = static_cast<int>(paintSizeHeight*factor + 0.5);
	painter.drawRect(centerDrawPoint.x()-sizeW, centerDrawPoint.y()-sizeH, sizeW*2, sizeH*2);
}

bool BScanSegLocalOpOperation::endOnCoord(int x, int y)
{
	cv::Mat* map = getActMat();
	if(!map || map->empty())
		return false;

	int rows = map->rows;
	int cols = map->cols;

	int x0 = std::max(x - paintSizeWidth , 0);
	int x1 = std::min(x + paintSizeWidth , cols-1);
	int y0 = std::max(y - paintSizeHeight, 0);
	int y1 = std::min(y + paintSizeHeight, rows-1);

	if(x0>=x1)
		return false;
	if(y0>=y1)
		return false;

	cv::Mat tmp = (*map)(cv::Rect(x0, y0, x1-x0, y1-y0));
	cv::Mat cpy = tmp.clone();

	int iterations = 1;

	switch(localOperation)
	{
		case BScanSegmentationMarker::Operation::Dilate:
			cv::dilate(cpy, tmp, cv::Mat(), cv::Point(-1, -1), iterations, cv::BORDER_REFLECT);
			break;
		case BScanSegmentationMarker::Operation::Erode:
			cv::erode(cpy, tmp, cv::Mat(), cv::Point(-1, -1), iterations, cv::BORDER_REFLECT);
			break;
		case BScanSegmentationMarker::Operation::OpenClose:
			BScanSegAlgorithm::openClose(tmp, &cpy);
			break;
		case BScanSegmentationMarker::Operation::Median:
			medianBlur(cpy, tmp, 3);
			break;
	}

	return true;
}
void BScanSegLocalOpOperation::setOperationData(const BScanSegmentationMarker::Operation& data)
{
	if(assignUpdateNecessary(localOperation, data))
		updateCursor();
}
void BScanSegLocalOpOperation::setOperatorSizeHeight(int size)
{
	size = validOperatorSize(size);
	if(assignUpdateNecessary(paintSizeHeight, size))
		updateCursor();
}
void BScanSegLocalOpOperation::setOperatorSizeWidth(int size)
{
	size = validOperatorSize(size);
	if(assignUpdateNecessary(paintSizeWidth, size))
		updateCursor();
}



// -------------------
// Threshold Direction
// -------------------
void BScanSegLocalOpThresholdDirection::drawMarkerPaint(QPainter& painter, const QPoint& centerDrawPoint, double factor) const
{
	int sizeW = static_cast<int>(paintSizeWidth *factor + 0.5);
	int sizeH = static_cast<int>(paintSizeHeight*factor + 0.5);
	painter.drawRect(centerDrawPoint.x()-sizeW, centerDrawPoint.y()-sizeH, sizeW*2, sizeH*2);
}


bool BScanSegLocalOpThresholdDirection::applyThreshold(int x, int y)
{
	cv::Mat* map = getActMat();
	if(!map || map->empty())
		return false;

	int rows = map->rows;
	int cols = map->cols;

	int x0 = std::max(x - paintSizeWidth , 0);
	int x1 = std::min(x + paintSizeWidth , cols-1);
	int y0 = std::max(y - paintSizeHeight, 0);
	int y1 = std::min(y + paintSizeHeight, rows-1);

	if(x0>=x1)
		return false;
	if(y0>=y1)
		return false;

	const OctData::BScan* bscan = getActBScan();
	if(!bscan)
		return false;

	cv::Mat tmp = (*map)(cv::Rect(x0, y0, x1-x0, y1-y0));
	cv::Mat tmpImages = bscan->getImage()(cv::Rect(x0, y0, x1-x0, y1-y0));

	BScanSegAlgorithm::initFromThresholdDirection(tmpImages, tmp, localThresholdData);

	return true;
}
void BScanSegLocalOpThresholdDirection::setOperatorSizeHeight(int size)
{
	size = validOperatorSize(size);
	if(assignUpdateNecessary(paintSizeHeight, size))
		updateCursor();
}
void BScanSegLocalOpThresholdDirection::setOperatorSizeWidth(int size)
{
	size = validOperatorSize(size);
	if(assignUpdateNecessary(paintSizeWidth, size))
		updateCursor();
}
void BScanSegLocalOpThresholdDirection::setThresholdData(const BScanSegmentationMarker::ThresholdDirectionData& data)
{
	if(assignUpdateNecessary(localThresholdData, data))
		updateCursor();
}



// ---------
// Threshold
// ---------

void BScanSegLocalOpThreshold::drawMarkerPaint(QPainter& painter, const QPoint& centerDrawPoint, double factor) const
{
	int sizeW = static_cast<int>(paintSizeWidth *factor + 0.5);
	int sizeH = static_cast<int>(paintSizeHeight*factor + 0.5);
	painter.drawRect(centerDrawPoint.x()-sizeW, centerDrawPoint.y()-sizeH, sizeW*2, sizeH*2);
}


bool BScanSegLocalOpThreshold::applyThreshold(int x, int y)
{
	cv::Mat* map = getActMat();
	if(!map || map->empty())
		return false;

	int rows = map->rows;
	int cols = map->cols;

	int x0 = std::max(x - paintSizeWidth , 0);
	int x1 = std::min(x + paintSizeWidth , cols-1);
	int y0 = std::max(y - paintSizeHeight, 0);
	int y1 = std::min(y + paintSizeHeight, rows-1);

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
void BScanSegLocalOpThreshold::setOperatorSizeHeight(int size)
{
	size = validOperatorSize(size);
	if(assignUpdateNecessary(paintSizeHeight, size))
		updateCursor();
}
void BScanSegLocalOpThreshold::setOperatorSizeWidth(int size)
{
	size = validOperatorSize(size);
	if(assignUpdateNecessary(paintSizeWidth, size))
		updateCursor();
}
void BScanSegLocalOpThreshold::setThresholdData(const BScanSegmentationMarker::ThresholdData& data)
{
	if(assignUpdateNecessary(localThresholdData, data))
		updateCursor();
}



