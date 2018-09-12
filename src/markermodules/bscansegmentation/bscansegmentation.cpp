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

#include "bscansegmentation.h"

#include <QPainter>
#include <QMouseEvent>
#include <QWidget>
#include<QDialog>
#include<QTextEdit>

#include <manager/octmarkermanager.h>

#include <widgets/bscanmarkerwidget.h>

#include <octdata/datastruct/bscan.h>
#include <octdata/datastruct/series.h>

#include <opencv/cv.h>

#include "bscansegmentationptree.h"

#include "wgsegmentation.h"
#include "bscansegalgorithm.h"
#include "bscansegtoolbar.h"

#include "bscanseglocalop.h"
#include "bscanseglocalopnn.h"

#include "paintsegmentationtotikz.h"

#include <data_structure/simplecvmatcompress.h>
#include <data_structure/scalefactor.h>
#include <data_structure/programoptions.h>
#include "simplemarchingsquare.h"
#include "freeformsegcommand.h"



BScanSegmentation::BScanSegmentation(OctMarkerManager* markerManager)
: BscanMarkerBase(markerManager)
, actMat(new cv::Mat)
{
	name = tr("Segmentation marker");
	id   = "SegmentationMarker";
	
	icon = QIcon(":/icons/typicons_mod/manual_seg.svg");

	localOpPaint              = new BScanSegLocalOpPaint    (*this);
	localOpThresholdDirection = new BScanSegLocalOpThresholdDirection(*this);
	localOpThreshold          = new BScanSegLocalOpThreshold(*this);
	localOpOperation          = new BScanSegLocalOpOperation(*this);
#ifdef ML_SUPPORT
	localOpNN                 = new BScanSegLocalOpNN       (*this);
#endif

	setLocalMethod(BScanSegmentationMarker::LocalMethod::Paint);

	widget = new WGSegmentation(this);
	widgetPtr2WGSegmentation = widget;

	
	connect(&ProgramOptions::freeFormedSegmetationShowArea, &OptionBool::trueSignal, this, &BScanSegmentation::updateAreaImageSlot);
	connect(&ProgramOptions::freeFormedSegmetationShowArea, &OptionBool::valueChanged, this, &BScanSegmentation::requestFullUpdate);
	// connect(markerManager, &BScanMarkerManager::newSeriesShowed, this, &BScanSegmentation::newSeriesLoaded);

	connect(&ProgramOptions::freeFormedSegmetationLineThickness, &OptionInt::valueChanged, this, &BScanSegmentation::requestFullUpdate);
}

BScanSegmentation::~BScanSegmentation()
{
	clearSegments();

	delete localOpPaint             ;
	delete localOpThresholdDirection;
	delete localOpThreshold         ;
	delete localOpOperation         ;
#ifdef ML_SUPPORT
	delete localOpNN                ;
#endif

	delete widget;
	delete actMat;
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
	template<typename Painter, typename Transformer>
	void drawSegmentLineRec(Painter& painter, Transformer& transform, const cv::Mat& actMat, int startH, int endH, int startW, int endW)
	{
		for(int h = startH; h < endH; ++h)
		{
			const uint8_t* p00 = actMat.ptr<uint8_t>(h);
			const uint8_t* p10 = p00+1;
			const uint8_t* p01 = actMat.ptr<uint8_t>(h+1);
			const uint8_t* p11 = p01+1;

			p00 += startW;
			p10 += startW;
			p01 += startW;
			p11 += startW;

			for(int w = startW; w < endW; ++w)
			{
				transform.handleSquare(*p01, *p11, *p10, *p00, h+1, w+1, painter);
// 				painter.paint(p00, p10, p01, w, h, mask, factor);

				++p00;
				++p10;
				++p01;
				++p11;
			}
// 			painter.paint(p00, p00, p01, endW, h, mask, factor); // last col p10 replaced by p00, because p10 is outside
		}
	}
	// TODO draw last row


	class SimplePaintTransform
	{
/*
    (3) ---- (2)
     |        |
     |        |
    (0) ---- (1)
*/
	public:
		void handleSquare(uint8_t gridVal0, uint8_t gridVal1, uint8_t gridVal2, uint8_t gridVal3, int i, int j, PaintSegLine& painter)
		{
			if(gridVal0 != gridVal3)
				painter.paintLine(Point2D(j, i), Point2D(j-1, i));
			if(gridVal2 != gridVal3)
				painter.paintLine(Point2D(j, i), Point2D(j, i-1));
		}
	};
}


template<typename Painter, typename Transformer>
void BScanSegmentation::drawSegmentLine(Painter& painter, Transformer& transform, const ScaleFactor& factor, const QRect& rect) const
{
	if(actMatNr != getActBScanNr())
	{
		qDebug("BScanSegmentation::drawSegmentLine: actMatNr != getActBScanNr()");
		return;
	}
	if(!actMat || actMat->empty())
		return;

	if(!factor.isValid())
		return;

	const double factorX = factor.getFactorX();
	const double factorY = factor.getFactorY();

	int drawX      = static_cast<int>((rect.x()     )/factorX + 0.5)-2;
	int drawY      = static_cast<int>((rect.y()     )/factorY + 0.5)-2;
	int drawWidth  = static_cast<int>((rect.width() )/factorX + 0.5)+4;
	int drawHeight = static_cast<int>((rect.height())/factorY + 0.5)+4;


	int mapHeight = actMat->rows-1; // -1 for p01
	int mapWidth  = actMat->cols-1; // -1 for p10

	int startH = std::max(drawY, 0);
	int endH   = std::min(drawY+drawHeight, mapHeight);
	int startW = std::max(drawX, 0);
	int endW   = std::min(drawX+drawWidth , mapWidth);

	QPen pen(Qt::red);
	pen.setWidth(ProgramOptions::freeFormedSegmetationLineThickness());
	painter.setPen(pen);
	drawSegmentLineRec(painter, transform, *actMat, startH, endH, startW, endW);
}


template<typename Painter>
void BScanSegmentation::drawSegmentLine(Painter& painter, const ScaleFactor& factor, const QRect& rect) const
{
	switch(viewMethod)
	{
		case ViewMethod::MarchingSquare:
		{
			SimpleMarchingSquare sms;
			drawSegmentLine(painter, sms, factor, rect);
			break;
		}
		case ViewMethod::Rect:
		{
			SimplePaintTransform spt;
			drawSegmentLine(painter, spt, factor, rect);
			break;
		}
	}
}

void BScanSegmentation::transformCoordWidget2Mat(int xWidget, int yWidget, const ScaleFactor& factor, int& xMat, int& yMat)
{
	xMat = static_cast<int>(xWidget/factor.getFactorX() + 0.5);
	yMat = static_cast<int>(yWidget/factor.getFactorY() + 0.5);
}


QString BScanSegmentation::generateTikzCode() const
{
	if(!actMat || actMat->empty())
		return QString();
	PaintSegmentationToTikz ptt(actMat->cols, actMat->rows);

	QRect fullRect(0,0,actMat->cols, actMat->rows);


	SimpleMarchingSquare sms;
	drawSegmentLine(ptt, sms, ScaleFactor(), fullRect);

	return ptt.getTikzCode();
}



void BScanSegmentation::drawMarker(QPainter& p, BScanMarkerWidget* widget, const QRect& rect) const
{
	const ScaleFactor& factor = widget->getImageScaleFactor();
	if(factor.getFactorX() <= 0 || factor.getFactorY() <= 0)
		return;

	if(ProgramOptions::freeFormedSegmetationShowArea())
		CVImageWidget::drawScaled(areaImage, p, &rect, factor);

	if(factor.isIdentical())
	{
		PaintFactor1 pf(p);
		drawSegmentLine(pf, factor, rect);
	}
	else
	{
		PaintFactorN pf(p, factor);
		drawSegmentLine(pf, factor, rect);
	}

	QPoint paintPoint = mousePoint;
	if(!factor.isIdentical())
	{
		int x = static_cast<int>(std::round(static_cast<double>(mousePoint.x())/factor.getFactorX())*factor.getFactorX() + 0.5);
		int y = static_cast<int>(std::round(static_cast<double>(mousePoint.y())/factor.getFactorY())*factor.getFactorY() + 0.5);
		paintPoint = QPoint(x, y);
	}

	if(inWidget && markerActive && actLocalOperator)
	{
		QPen pen(Qt::green);
		p.setPen(pen);
		actLocalOperator->drawMarkerPaint(p, paintPoint, factor);
	}
}


bool BScanSegmentation::setOnCoord(int x, int y, const ScaleFactor& factor)
{
	if(!factor.isValid())
		return false;

	int xD, yD;
	transformCoordWidget2Mat(x, y, factor, xD, yD);

	if(actLocalOperator)
		return actLocalOperator->drawOnCoord(xD, yD);

	return false;
}



bool BScanSegmentation::startOnCoord(int x, int y, const ScaleFactor& factor)
{
	if(!factor.isValid())
		return false;

	int xD, yD;
	transformCoordWidget2Mat(x, y, factor, xD, yD);

	if(actLocalOperator)
		actLocalOperator->startOnCoord(xD, yD);

	return true;
}


uint8_t BScanSegmentation::valueOnCoord(int x, int y)
{
	setActMat(getActBScanNr());
	if(!actMat || actMat->empty())
		return BScanSegmentationMarker::markermatInitialValue;

	if(x < 0 || y < 0 || x >= actMat->cols || y >= actMat->rows)
		return 0;

	return actMat->at<uint8_t>(cv::Point(x, y));
}


QRect BScanSegmentation::getWidgetPaintSize(const QPoint& p1, const QPoint& p2, const ScaleFactor& factor)
{
	int height = 2;
	int width  = 2;
	if(actLocalOperator)
	{
		height += actLocalOperator->getOperatorHeight();
		width  += actLocalOperator->getOperatorWidth ();
	}

	height = static_cast<int>(height*factor.getFactorY() + 0.5);
	width  = static_cast<int>(width *factor.getFactorX() + 0.5);

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

		const ScaleFactor& factor = widget->getImageScaleFactor();

		result.redraw = false; // cursor need redraw
		result.rect   = getWidgetPaintSize(mousePoint, e->pos(), factor);

		int x = e->x();
		int y = e->y();

		if(actLocalOperator)
		{
			if(paint)
				result.redraw = setOnCoord(x, y, factor);

			result.redraw |= actLocalOperator->drawMarker();

			updateAreaImage(result, factor);
		}
	}
	mousePoint = e->pos();

	return result;
}

BscanMarkerBase::RedrawRequest  BScanSegmentation::mousePressEvent(QMouseEvent* e, BScanMarkerWidget* widget)
{
	const ScaleFactor& factor = widget->getImageScaleFactor();

	RedrawRequest result;
	result.redraw = false;
	result.rect   = getWidgetPaintSize(mousePoint, e->pos(), factor);

	paint = (e->buttons() == Qt::LeftButton);
	if(paint)
	{
		startOnCoord(e->x(), e->y(), factor);
		result.redraw = setOnCoord(e->x(), e->y(), factor);

		updateAreaImage(result, factor);
	}
	return result;
}

BscanMarkerBase::RedrawRequest  BScanSegmentation::mouseReleaseEvent(QMouseEvent* e, BScanMarkerWidget* widget)
{
	const ScaleFactor& factor = widget->getImageScaleFactor();

	RedrawRequest result;
	result.redraw = false;
	result.rect   = getWidgetPaintSize(mousePoint, e->pos(), factor);

	paint = false;
	if(!factor.isValid())
		return result;

	if(actLocalOperator)
	{
		int x = e->x();
		int y = e->y();

		int xD, yD;
		transformCoordWidget2Mat(x, y, factor, xD, yD);

		result.redraw = actLocalOperator->endOnCoord(xD, yD);
		updateAreaImage(result, factor);
		createUndoStep();
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
		case Qt::Key_U:
			removeUnconectedAreas();
			return true;
		case Qt::Key_I:
			extendLeftRightSpace();
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
		case Qt::Key_5:
			setLocalMethod(BScanSegmentationMarker::LocalMethod::NN);
			return true;
		case Qt::Key_G:
			showTikzCode();
			return true;
		case Qt::Key_K:
			if(viewMethod == ViewMethod::MarchingSquare)
				viewMethod = ViewMethod::Rect;
			else
				viewMethod = ViewMethod::MarchingSquare;
			return true;
	}

	return false;
}





void BScanSegmentation::dilateBScan()
{
	setActMat(getActBScanNr());
	if(!actMat || actMat->empty())
		return;

	int iterations = 1;
	cv::dilate(*actMat, *actMat, cv::Mat(), cv::Point(-1, -1), iterations, cv::BORDER_REFLECT_101, 1);

	createUndoStep();
	updateAreaImage(areaImage.rect());
	requestFullUpdate();
}

void BScanSegmentation::erodeBScan()
{
	setActMat(getActBScanNr());
	if(!actMat || actMat->empty())
		return;

	int iterations = 1;
	cv::erode(*actMat, *actMat, cv::Mat(), cv::Point(-1, -1), iterations, cv::BORDER_REFLECT_101, 1);

	createUndoStep();
	updateAreaImage(areaImage.rect());
	requestFullUpdate();
}

void BScanSegmentation::opencloseBScan()
{
	setActMat(getActBScanNr());
	if(!actMat || actMat->empty())
		return;

	BScanSegAlgorithm::openClose(*actMat);

	createUndoStep();
	updateAreaImage(areaImage.rect());
	requestFullUpdate();
}


void BScanSegmentation::medianBScan()
{
	setActMat(getActBScanNr());
	if(!actMat || actMat->empty())
		return;

	medianBlur(*actMat, *actMat, 3);

	createUndoStep();
	updateAreaImage(areaImage.rect());
	requestFullUpdate();
}

void BScanSegmentation::removeUnconectedAreas()
{
	if(!actMat)
		return;

	if(BScanSegAlgorithm::removeUnconectedAreas(*actMat))
	{
		updateAreaImage(areaImage.rect());
		requestFullUpdate();
	}
}

void BScanSegmentation::extendLeftRightSpace()
{
	if(!actMat)
		return;

	if(BScanSegAlgorithm::extendLeftRightSpace(*actMat))
	{
		requestFullUpdate();
		updateAreaImage(areaImage.rect());
	}
}

void BScanSegmentation::seriesRemoveUnconectedAreas()
{
	const OctData::Series* series = getSeries();
	for(std::size_t i=0; i<series->bscanCount(); ++i)
	{
		if(setActMat(i))
		{
			if(actMat)
				BScanSegAlgorithm::removeUnconectedAreas(*actMat);
		}
	}
	setActMat(getActBScanNr());
	updateAreaImage(areaImage.rect());
	requestFullUpdate();
}

void BScanSegmentation::seriesExtendLeftRightSpace()
{
	const OctData::Series* series = getSeries();
	for(std::size_t i=0; i<series->bscanCount(); ++i)
	{
		if(setActMat(i))
		{
			if(actMat)
				BScanSegAlgorithm::extendLeftRightSpace(*actMat);
		}
	}
	setActMat(getActBScanNr());
	updateAreaImage(areaImage.rect());
	requestFullUpdate();
}




void BScanSegmentation::clearSegments()
{
	for(auto mat : segments)
		delete mat;

	segments.clear();
}

void BScanSegmentation::createSegments()
{
	const OctData::Series* series = getSeries();

	if(!series)
		return;

	clearSegments();
	createSegments(series);
}


void BScanSegmentation::createSegments(const OctData::Series* series)
{
	if(!series)
		return;

	clearSegments();

	for(std::size_t i=0; i<series->bscanCount(); ++i)
	{
		SimpleCvMatCompress* mat;
		const OctData::BScan* bscan = getBScan(i);
		if(bscan)
			mat = new SimpleCvMatCompress(bscan->getHeight(), bscan->getWidth(), BScanSegmentationMarker::markermatInitialValue);
		else
			mat = new SimpleCvMatCompress;
		segments.push_back(mat);
	}
}


void BScanSegmentation::newSeriesLoaded(const OctData::Series* series, boost::property_tree::ptree& markerTree)
{
	BscanMarkerBase::newSeriesLoaded(series, markerTree);

	if(!series)
		return;
	createSegments(series);
	loadState(markerTree);
}

void BScanSegmentation::saveState(boost::property_tree::ptree& markerTree)
{
	BscanMarkerBase::saveState(markerTree);

	createUndoStep();
	BScanSegmentationPtree::fillPTree(markerTree, this);
	stateChangedSinceLastSave = false;
}

void BScanSegmentation::loadState(boost::property_tree::ptree& markerTree)
{
	BscanMarkerBase::loadState(markerTree);

	BScanSegmentationPtree::parsePTree(markerTree, this);
	setActMat(getActBScanNr(), false);
	stateChangedSinceLastSave = false;
}


void BScanSegmentation::updateCursor()
{
	if(inWidget)
		requestFullUpdate();
}


void BScanSegmentation::initBScanFromThreshold(const BScanSegmentationMarker::ThresholdDirectionData& data)
{
	setActMat(getActBScanNr());
	if(!actMat || actMat->empty())
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


	BScanSegAlgorithm::initFromThresholdDirection(image, *actMat, data, BScanSegmentationMarker::paintArea0Value, BScanSegmentationMarker::paintArea1Value);

	updateAreaImage(areaImage.rect());
	requestFullUpdate();
}


void BScanSegmentation::initSeriesFromThreshold(const BScanSegmentationMarker::ThresholdDirectionData& data)
{
	const OctData::Series* series = getSeries();

	std::size_t bscanCount = 0;
	for(const OctData::BScan* bscan : series->getBScans())
	{
		const cv::Mat& image = bscan->getImage();

		if(setActMat(bscanCount))
		{
			if(actMat && !actMat->empty())
				BScanSegAlgorithm::initFromThresholdDirection(image, *actMat, data, BScanSegmentationMarker::paintArea0Value, BScanSegmentationMarker::paintArea1Value);
		}
		++bscanCount;
	}
	setActMat(getActBScanNr());
	updateAreaImage(areaImage.rect());
	requestFullUpdate();
}

void BScanSegmentation::initBScanFromSegline(OctData::Segmentationlines::SegmentlineType type)
{
	setActMat(getActBScanNr());
	if(!actMat || actMat->empty())
		return;

	const OctData::Series* series = getSeries();
	if(!series)
		return;

	const OctData::BScan* bscan = series->getBScan(getActBScanNr());
	if(!bscan)
		return;

	BScanSegAlgorithm::initFromSegline(*bscan, *actMat, type);

	updateAreaImage(areaImage.rect());
	requestFullUpdate();
}

void BScanSegmentation::initSeriesFromSegline(OctData::Segmentationlines::SegmentlineType type)
{
	const OctData::Series* series = getSeries();


	std::size_t bscanCount = 0;
	for(const OctData::BScan* bscan : series->getBScans())
	{
		if(!bscan)
			continue;

		if(setActMat(bscanCount))
		{
			if(actMat && !actMat->empty())
				BScanSegAlgorithm::initFromSegline(*bscan, *actMat, type);

		}
		++bscanCount;
	}
	setActMat(getActBScanNr());
	updateAreaImage(areaImage.rect());
	requestFullUpdate();
}



void BScanSegmentation::setLocalMethod(BScanSegmentationMarker::LocalMethod method)
{
	if(localMethod != method)
	{
		localMethod = method;
		switch(localMethod)
		{
			case BScanSegmentationMarker::LocalMethod::None:
				actLocalOperator = nullptr;
				break;
			case BScanSegmentationMarker::LocalMethod::Paint:
				actLocalOperator = localOpPaint;
				break;
			case BScanSegmentationMarker::LocalMethod::Operation:
				actLocalOperator = localOpOperation;
				break;
			case BScanSegmentationMarker::LocalMethod::Threshold:
				actLocalOperator = localOpThreshold;
				break;
			case BScanSegmentationMarker::LocalMethod::NN:
				actLocalOperator = localOpNN;
				break;
			case BScanSegmentationMarker::LocalMethod::ThresholdDirection:
				actLocalOperator = localOpThresholdDirection;
				break;
		}
		localOperatorChanged(method);
	}
}

bool BScanSegmentation::setActMat(std::size_t nr, bool saveOldState)
{
	if(nr == actMatNr && saveOldState)
		return true;

	if(actMat)
	{
		if(saveOldState)
			createUndoStep(); // save state from old bscan

		if(segments.size() > nr)
		{
			segments[nr]->writeToMat(*actMat); // load state from new bscan
			actMatNr = nr;

			if(actMat->empty())
			{
				const OctData::BScan* bscan = getBScan(nr);
				if(bscan)
				{
					*actMat = cv::Mat(bscan->getHeight(), bscan->getWidth(), cv::DataType<uint8_t>::type, cvScalar(BScanSegmentationMarker::markermatInitialValue));
				}
			}
			areaImage = QImage(QSize(actMat->cols, actMat->rows), QImage::Format_ARGB32_Premultiplied);
			updateAreaImage(areaImage.rect());
			return true;
		}
	}
	return false;
}


void BScanSegmentation::updateAreaImageSlot()
{
	updateAreaImage(areaImage.rect());
}

void BScanSegmentation::updateAreaImage(const RedrawRequest& redraw, const ScaleFactor& factor)
{
	QRect rect(redraw.rect.x()/factor.getFactorX(), redraw.rect.y()/factor.getFactorY(), redraw.rect.width()/factor.getFactorX(), redraw.rect.height()/factor.getFactorY());
	updateAreaImage(rect);
}

void BScanSegmentation::updateAreaImage(const QRect& rect)
{
	if(!ProgramOptions::freeFormedSegmetationShowArea())
		return;

	if(areaImage.width() != actMat->cols || areaImage.height() != actMat->rows)
		return;

	QRgb f1 = qRgba(128, 0, 0, 128);
	QRgb f2 = qRgba(0, 0, 0, 0);

	int startX = std::max(0, rect.x());
	int startY = std::max(0, rect.y());
	int endX   = std::min(actMat->cols, rect.x() + rect.width());
	int endY   = std::min(actMat->rows, rect.y() + rect.height());

	for(int row = startY; row < endY; ++row)
	{
		QRgb* outPtr = reinterpret_cast<QRgb*>(areaImage.scanLine(row)) + startX;
		const BScanSegmentationMarker::internalMatType* inPtr = actMat->ptr<BScanSegmentationMarker::internalMatType>(row) + startX;

		for(int col = startX; col < endX; ++col)
		{
			if(*inPtr)
				*outPtr = f1;
			else
				*outPtr = f2;
			++outPtr;
			++inPtr;
		}
	}
}

void BScanSegmentation::createUndoStep()
{
	if(actMat && segments.size() > actMatNr)
	{
		SimpleCvMatCompress newMat;
		newMat.readFromMat(*actMat);

		if((*segments[actMatNr]) != newMat)
		{
			stateChangedSinceLastSave = true;

			FreeFormSegCommand* command = new FreeFormSegCommand(*this, *(segments[actMatNr]));
			addUndoCommand(command);

			*(segments[actMatNr]) = newMat;
		}
	}
}


bool BScanSegmentation::swapActMat(SimpleCvMatCompress& otherMat)
{
	if(!actMat)
		return false;

	SimpleCvMatCompress oldMat;
	oldMat.readFromMat(*actMat);
	otherMat.writeToMat(*actMat);

	*(segments[actMatNr]) = otherMat;

	std::swap(oldMat, otherMat);

	updateAreaImage(areaImage.rect());
	requestFullUpdate();
	return true;
}


bool BScanSegmentation::hasActMatChanged() const
{
	if(actMat && segments.size() > actMatNr)
		return (*segments[actMatNr]) != *actMat;
	return false;
}


void BScanSegmentation::showTikzCode()
{
	QString code = generateTikzCode();

	QDialog dialog;
	dialog.setWindowModality(Qt::ApplicationModal);
	QVBoxLayout* vbox = new QVBoxLayout(&dialog);
	QTextEdit* te = new QTextEdit(&dialog);
	te->setPlainText(code);

	vbox->addWidget(te);

	dialog.exec();
}

