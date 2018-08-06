#include "bscanseglocalop.h"

#include "bscansegmentation.h"
#include "bscansegalgorithm.h"

#include <QPainter>
#include <opencv/cv.h>
#if CV_MAJOR_VERSION >= 3
	#include <opencv2/imgproc.hpp>
#endif
#include <octdata/datastruct/bscan.h>



BScanSegmentationMarker::ColorData BScanSegLocalOp::localColorData;

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


BScanSegmentationMarker::internalMatType BScanSegLocalOp::getStartPaintColor(int x, int y)
{
	switch(localColorData.paintColor)
	{
		case BScanSegmentationMarker::ColorData::PaintColor::Auto:
			return valueOnCoord(x, y);
		case BScanSegmentationMarker::ColorData::PaintColor::Area0:
			return BScanSegmentationMarker::paintArea0Value;
		case BScanSegmentationMarker::ColorData::PaintColor::Area1:
			return BScanSegmentationMarker::paintArea1Value;
	}
	return BScanSegmentationMarker::paintArea0Value;
}

QIcon BScanSegLocalOp::getPaintColorIcon(BScanSegmentationMarker::ColorData::PaintColor color)
{
	switch(color)
	{
		case BScanSegmentationMarker::ColorData::PaintColor::Area0:
			return createMonocromeIcon(255);
		case BScanSegmentationMarker::ColorData::PaintColor::Auto:
			return createMonocromeIcon(124);
		case BScanSegmentationMarker::ColorData::PaintColor::Area1:
			return createMonocromeIcon(0);
	}
	return QIcon();
}

BScanSegmentationMarker::internalMatType BScanSegLocalOp::getOtherPaintValue(BScanSegmentationMarker::internalMatType v)
{
	if(v == BScanSegmentationMarker::paintArea0Value)
		return BScanSegmentationMarker::paintArea1Value;
	if(v == BScanSegmentationMarker::paintArea1Value)
		return BScanSegmentationMarker::paintArea0Value;
	return BScanSegmentationMarker::paintArea1Value; // TODO: error handling
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


void BScanSegLocalOpPaint::drawMarkerPaint(QPainter& painter, const QPoint& centerDrawPoint, const ScaleFactor& factor) const
{
	const double factorX = factor.getFactorX();
	const double factorY = factor.getFactorY();

	switch(localPaintData.paintMethod)
	{
		case BScanSegmentationMarker::PaintData::PaintMethod::Circle:
		{
			QPointF point = centerDrawPoint;
			point += QPointF(factorX/2, factorY/2);
			painter.drawEllipse(point
			                  , (paintSize+0.5)*factorX
			                  , (paintSize+0.5)*factorY);
			/*
			painter.drawEllipse(centerDrawPoint + QPoint(static_cast<int>(factor/2), static_cast<int>(factor/2))
			                  , (paintSize+0.5)*factor
			                  , (paintSize+0.5)*factor);*/
			break;
		}
		case BScanSegmentationMarker::PaintData::PaintMethod::Rect:
		{
			int sizeX = static_cast<int>(paintSize*factorX + 0.5);
			int sizeY = static_cast<int>(paintSize*factorY + 0.5);
			painter.drawRect(centerDrawPoint.x()-sizeX, centerDrawPoint.y()-sizeY, sizeX*2, sizeY*2);
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
			if(x>0 && y>0 && x<map->cols && y<map->rows)
				map->at<BScanSegmentationMarker::internalMatType>(y-1, x-1) = paintValue;
			break;
	}
	return true;
}

bool BScanSegLocalOpPaint::startOnCoord(int x, int y)
{
	paintValue = getStartPaintColor(x, y);
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



// ---------
// Operation
// ---------
void BScanSegLocalOpOperation::drawMarkerPaint(QPainter& painter, const QPoint& centerDrawPoint, const ScaleFactor& factor) const
{
	int sizeW = static_cast<int>(paintSizeWidth *factor.getFactorX() + 0.5);
	int sizeH = static_cast<int>(paintSizeHeight*factor.getFactorY() + 0.5);
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
void BScanSegLocalOpThresholdDirection::drawMarkerPaint(QPainter& painter, const QPoint& centerDrawPoint, const ScaleFactor& factor) const
{
	int sizeW = static_cast<int>(paintSizeWidth *factor.getFactorX() + 0.5);
	int sizeH = static_cast<int>(paintSizeHeight*factor.getFactorY() + 0.5);
	painter.drawRect(centerDrawPoint.x()-sizeW, centerDrawPoint.y()-sizeH, sizeW*2, sizeH*2);
}

bool BScanSegLocalOpThresholdDirection::startOnCoord(int x, int y)
{
	switch(localThresholdData.direction)
	{
		case BScanSegmentationMarker::ThresholdDirectionData::Direction::down : y -= paintSizeHeight; break;
		case BScanSegmentationMarker::ThresholdDirectionData::Direction::up   : y += paintSizeHeight; break;
		case BScanSegmentationMarker::ThresholdDirectionData::Direction::right: x -= paintSizeWidth ; break;
		case BScanSegmentationMarker::ThresholdDirectionData::Direction::left : x += paintSizeWidth ; break;
	}

	val1 = getStartPaintColor(x, y);
	val2 = getOtherPaintValue(val1);

	return true;
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

	BScanSegAlgorithm::initFromThresholdDirection(tmpImages, tmp, localThresholdData, val1, val2);

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

void BScanSegLocalOpThreshold::drawMarkerPaint(QPainter& painter, const QPoint& centerDrawPoint, const ScaleFactor& factor) const
{
	int sizeW = static_cast<int>(paintSizeWidth *factor.getFactorX() + 0.5);
	int sizeH = static_cast<int>(paintSizeHeight*factor.getFactorY() + 0.5);
	painter.drawRect(centerDrawPoint.x()-sizeW, centerDrawPoint.y()-sizeH, sizeW*2, sizeH*2);
}

bool BScanSegLocalOpThreshold::startOnCoord(int x, int y)
{
	val1 = getStartPaintColor(x, y);
	val2 = getOtherPaintValue(val1);

	if(localColorData.paintColor == BScanSegmentationMarker::ColorData::PaintColor::Auto)
	{
		cv::Mat segMat;
		cv::Mat imageMat;
		bool result = getLocalImageMat(x, y, imageMat, segMat);
		if(result)
		{
			const BScanSegmentationMarker::internalMatType grayValue    = BScanSegAlgorithm::getThresholdGrayValue(imageMat, localThresholdData);
			const BScanSegmentationMarker::internalMatType centralValue = imageMat.at<BScanSegmentationMarker::internalMatType>(imageMat.cols/2, imageMat.rows/2);

			if(grayValue < centralValue)
				std::swap(val1, val2);
		}
	}

	return true;
}


bool BScanSegLocalOpThreshold::getLocalImageMat(int x, int y, cv::Mat& image, cv::Mat& seg)
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

	seg   = (*map)(cv::Rect(x0, y0, x1-x0, y1-y0));
	image = bscan->getImage()(cv::Rect(x0, y0, x1-x0, y1-y0));
	return true;
}


bool BScanSegLocalOpThreshold::applyThreshold(int x, int y)
{
	cv::Mat segMat;
	cv::Mat imageMat;
	bool result = getLocalImageMat(x, y, imageMat, segMat);
	if(result)
		BScanSegAlgorithm::initFromThreshold(imageMat, segMat, localThresholdData, val1, val2);
	return result;
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



