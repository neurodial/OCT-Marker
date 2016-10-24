#include "bscansegmentation.h"

#include <QPainter>
#include <QMouseEvent>
#include <QIcon>
#include <QActionGroup>
#include <QAction>
#include <QToolBar>
#include <QSpinBox>

#include <manager/bscanmarkermanager.h>

#include <widgets/bscanmarkerwidget.h>

#include <octdata/datastruct/bscan.h>
#include <octdata/datastruct/series.h>

#include <opencv/cv.h>

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





BScanSegmentation::BScanSegmentation(BScanMarkerManager* markerManager)
: BscanMarkerBase(markerManager)
{
	name = tr("Segmentation marker");
	
	connect(markerManager, &BScanMarkerManager::newSeriesShowed, this, &BScanSegmentation::newSeriesLoaded);
}


void BScanSegmentation::drawSegmentLine2(QPainter& painter, int factor)
{
	cv::Mat* map = segments.at(getActBScan());
	if(!map || map->empty())
		return;


	
	QPen pen(Qt::red);
	painter.setPen(pen);
	
	int height = map->rows;
	int width  = map->cols;
	
	for(int h = 0; h < height-1; ++h)
	{
		uint8_t* p00 = map->ptr<uint8_t>(h);
		uint8_t* p10 = p00+1;
		uint8_t* p01 = map->ptr<uint8_t>(h+1);
		
		for(int w = 0; w < width-1; ++w)
		{
			if(*p00 != *p10)
				painter.drawLine((w+1)*factor, (h)*factor, (w+1)*factor, (h+1)*factor);
			if(*p00 != *p01)
				painter.drawLine((w)*factor, (h+1)*factor, (w+1)*factor, (h+1)*factor);
			
			++p00;
			++p10;
			++p01;
		}
	}
}


void BScanSegmentation::drawSegmentLine(QPainter& painter, int factor)
{
	if(factor > 1)
	{
		drawSegmentLine2(painter, factor);
		return;
	}
	
	cv::Mat* map = segments.at(getActBScan());
	if(!map || map->empty())
		return;
	
	
	QPen pen(Qt::red);
	painter.setPen(pen);
	
	int height = map->rows;
	int width  = map->cols;
	
	for(int h = 0; h < height-1; ++h)
	{
		uint8_t* p00 = map->ptr<uint8_t>(h);
		uint8_t* p10 = p00+1;
		uint8_t* p01 = map->ptr<uint8_t>(h+1);
		
		for(int w = 0; w < width-1; ++w)
		{
			if(*p00 != *p10)
				painter.drawPoint(w*factor, h*factor);
			else if(*p00 != *p01)
				painter.drawPoint(w*factor, h*factor);
			
			++p00;
			++p10;
			++p01;
		}
	}
	
}


void BScanSegmentation::drawMarker(QPainter& p, BScanMarkerWidget* widget)
{
	int factor = widget->getImageScaleFactor();
	if(factor == 0)
		return;
	
	drawSegmentLine(p, factor);
	
	QPen pen(Qt::green);
	p.setPen(pen);
	if(inWidget)
		p.drawEllipse(mousePoint, paintRadius*factor, paintRadius*factor);
}

QToolBar* BScanSegmentation::createToolbar(QObject* parent)
{
	QActionGroup*  actionGroupMethod  = new QActionGroup(parent);
	QToolBar*      toolBar            = new QToolBar(tr("Segmentation"));
	
	toolBar->setObjectName(tr("ToolBarSegmentationMarker"));
	
	QWidget* parentWidget = dynamic_cast<QWidget*>(parent);
	
	QSpinBox* paintSizeSpinBox = new QSpinBox(parentWidget);
	paintSizeSpinBox->setMinimum(1);
	paintSizeSpinBox->setValue(paintRadius);
	connect(paintSizeSpinBox, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, &BScanSegmentation::setPaintRadius);
	toolBar->addWidget(paintSizeSpinBox);
	
	
	QAction* addAreaAction = new QAction(parent);
	addAreaAction->setCheckable(true);
	addAreaAction->setText(tr("Add"));
	addAreaAction->setIcon(createMonocromeIcon(paintArea0Value*255));
	connect(addAreaAction, &QAction::triggered, this, &BScanSegmentation::paintArea0Slot);
	toolBar->addAction(addAreaAction);
	actionGroupMethod->addAction(addAreaAction);

	QAction* autoRemoveAddAreaAction = new QAction(parent);
	autoRemoveAddAreaAction->setCheckable(true);
	autoRemoveAddAreaAction->setChecked(autoPaintValue);
	autoRemoveAddAreaAction->setText(tr("Auto"));
	autoRemoveAddAreaAction->setIcon(createMonocromeIcon(124));
	connect(autoRemoveAddAreaAction, &QAction::triggered, this, &BScanSegmentation::autoAddRemoveArea);
	toolBar->addAction(autoRemoveAddAreaAction);
	actionGroupMethod->addAction(autoRemoveAddAreaAction);
	
	QAction* removeAreaAction = new QAction(parent);
	removeAreaAction->setCheckable(true);
	removeAreaAction->setText(tr("Add"));
	removeAreaAction->setIcon(createMonocromeIcon(paintArea1Value*255));
	connect(removeAreaAction, &QAction::triggered, this, &BScanSegmentation::paintArea1Slot);
	toolBar->addAction(removeAreaAction);
	actionGroupMethod->addAction(removeAreaAction);
	
	toolBar->addSeparator();
	
	QAction* actionInitFromIlm = new QAction(parent);
	actionInitFromIlm->setText(tr("Init from ILM"));
	actionInitFromIlm->setIcon(QIcon(":/icons/chart_curve.png"));
	connect(actionInitFromIlm, &QAction::triggered, this, &BScanSegmentation::initFromSegmentline);
	toolBar->addAction(actionInitFromIlm);
	
	return toolBar;
}

bool BScanSegmentation::setOnCoord(int x, int y, int factor)
{
	if(factor == 0)
		return false;
	
	cv::Mat* map = segments.at(getActBScan());
	if(!map || map->empty())
		return false;
	cv::circle(*map, cv::Point(x/factor, y/factor), paintRadius, paintValue, CV_FILLED, 8, 0);
	return true;
}

uint8_t BScanSegmentation::valueOnCoord(int x, int y, int factor)
{
	if(factor == 0)
		return 0;
	
	cv::Mat* map = segments.at(getActBScan());
	if(!map || map->empty())
		return 0;
	
	return map->at<uint8_t>(cv::Point(x/factor, y/factor));
}


bool BScanSegmentation::mouseMoveEvent(QMouseEvent* e, BScanMarkerWidget* widget)
{
	if(!(e->buttons() & Qt::LeftButton))
		paint = false;
		
	inWidget = true;
	mousePoint = e->pos();
	
	int factor = widget->getImageScaleFactor();
	
	if(paint)
		return setOnCoord(e->x(), e->y(), factor);
	return true;
}

bool BScanSegmentation::leaveWidgetEvent(QEvent*, BScanMarkerWidget*)
{
	inWidget = false;
	return true;
}

bool BScanSegmentation::keyPressEvent(QKeyEvent*, BScanMarkerWidget*)
{
	return false;
}

bool BScanSegmentation::mousePressEvent(QMouseEvent* e, BScanMarkerWidget* widget)
{
	paint = (e->buttons() == Qt::LeftButton);
	if(paint)
	{
		if(autoPaintValue)
			paintValue = valueOnCoord(e->x(), e->y(), widget->getImageScaleFactor());
		
		return setOnCoord(e->x(), e->y(), widget->getImageScaleFactor());
	}
	return false;
}

bool BScanSegmentation::mouseReleaseEvent(QMouseEvent*, BScanMarkerWidget*)
{
	paint = false;
	return false;
}

void BScanSegmentation::newSeriesLoaded(const OctData::Series* series)
{
	clearSegments();
	
	if(!series)
		return;
	
	for(const OctData::BScan* bscan : series->getBScans())
	{
		cv::Mat* mat = new cv::Mat(bscan->getHeight(), bscan->getWidth(), cv::DataType<uint8_t>::type, cvScalar(0));
		segments.push_back(mat);
	}
}

void BScanSegmentation::clearSegments()
{
	for(cv::Mat* mat : segments)
		delete mat;
	
	segments.clear();
}

void BScanSegmentation::initFromSegmentline()
{
	const OctData::Series* series = getSeries();
	
	SegMats::iterator segMatIt = segments.begin();
	
	for(const OctData::BScan* bscan : series->getBScans())
	{
		const OctData::BScan::Segmentline& segline = bscan->getSegmentLine(OctData::BScan::SegmentlineType::ILM);
		cv::Mat* mat = *segMatIt;
		if(mat && !mat->empty())
		{
			internalMatType* colIt = mat->ptr<internalMatType>();
			std::size_t colSize = static_cast<std::size_t>(mat->cols);
			std::size_t rowSize = static_cast<std::size_t>(mat->rows);
			for(double value : segline)
			{
				const std::size_t rowCh = std::min(static_cast<std::size_t>(value), rowSize);
				internalMatType* rowIt = colIt;

				for(std::size_t row = 0; row < rowCh; ++row)
				{
					*rowIt =  1;
					rowIt += colSize;
				}
				
				for(std::size_t row = rowCh; row < rowSize; ++row)
				{
					*rowIt = 0;
					rowIt += colSize;
				}
				
				++colIt;
			}
		}
		++segMatIt;
	}
}

