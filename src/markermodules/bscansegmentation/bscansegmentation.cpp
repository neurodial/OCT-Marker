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

#include "bscansegmentationptree.h"

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
	id   = "SegmentationMarker";
	
	icon = QIcon(":/icons/segline_edit.png");
	
	// connect(markerManager, &BScanMarkerManager::newSeriesShowed, this, &BScanSegmentation::newSeriesLoaded);
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
	{
		switch(paintMethod)
		{
			case PaintMethod::Disc:
				p.drawEllipse(mousePoint, paintRadius*factor, paintRadius*factor);
				break;
			case PaintMethod::Quadrat:
			{
				int size = paintRadius*factor;
				p.drawRect(mousePoint.x()-size, mousePoint.y()-size, size*2, size*2);
				break;
			}
		}
	}
}

QToolBar* BScanSegmentation::createToolbar(QObject* parent)
{
	QActionGroup*  actionGroupMethod  = new QActionGroup(parent);
	QActionGroup*  actionPaintMethod  = new QActionGroup(parent);
	QToolBar*      toolBar            = new QToolBar(tr("Segmentation"));
	
	toolBar->setObjectName("ToolBarSegmentationMarker");
	
	QWidget* parentWidget = dynamic_cast<QWidget*>(parent);
	
	QSpinBox* paintSizeSpinBox = new QSpinBox(parentWidget);
	paintSizeSpinBox->setMinimum(1);
	paintSizeSpinBox->setValue(paintRadius);
	connect(paintSizeSpinBox, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, &BScanSegmentation::setPaintRadius);
	toolBar->addWidget(paintSizeSpinBox);

	QAction* actionPaintMethodDisc = new QAction(parent);
	actionPaintMethodDisc->setCheckable(true);
	actionPaintMethodDisc->setChecked(paintMethod == PaintMethod::Disc);
	actionPaintMethodDisc->setText(tr("Disc"));
	actionPaintMethodDisc->setIcon(QIcon(":/icons/paint_disc.png"));
	connect(actionPaintMethodDisc, &QAction::triggered, this, &BScanSegmentation::setPaintMethodDisc);
	connect(this, &BScanSegmentation::paintArea1Selected, actionPaintMethodDisc, &QAction::setChecked);
	toolBar->addAction(actionPaintMethodDisc);
	actionPaintMethod->addAction(actionPaintMethodDisc);


	QAction* actionPaintMethodQuadrat = new QAction(parent);
	actionPaintMethodQuadrat->setCheckable(true);
	actionPaintMethodQuadrat->setText(tr("Quadrat"));
	actionPaintMethodQuadrat->setChecked(paintMethod == PaintMethod::Quadrat);
	actionPaintMethodQuadrat->setIcon(QIcon(":/icons/paint_quadrat.png"));
	connect(actionPaintMethodQuadrat, &QAction::triggered, this, &BScanSegmentation::setPaintMethodQuadrat);
	connect(this, &BScanSegmentation::paintArea1Selected, actionPaintMethodQuadrat, &QAction::setChecked);
	toolBar->addAction(actionPaintMethodQuadrat);
	actionPaintMethod->addAction(actionPaintMethodQuadrat);

	toolBar->addSeparator();
	
	QAction* addAreaAction = new QAction(parent);
	addAreaAction->setCheckable(true);
	addAreaAction->setText(tr("Add"));
	addAreaAction->setIcon(createMonocromeIcon(paintArea0Value*255));
	connect(addAreaAction, &QAction::triggered, this, &BScanSegmentation::paintArea0Slot);
	connect(this, &BScanSegmentation::paintArea0Selected, addAreaAction, &QAction::setChecked);
	toolBar->addAction(addAreaAction);
	actionGroupMethod->addAction(addAreaAction);

	QAction* autoRemoveAddAreaAction = new QAction(parent);
	autoRemoveAddAreaAction->setCheckable(true);
	autoRemoveAddAreaAction->setChecked(autoPaintValue);
	autoRemoveAddAreaAction->setText(tr("Auto"));
	autoRemoveAddAreaAction->setIcon(createMonocromeIcon(124));
	connect(autoRemoveAddAreaAction, &QAction::triggered, this, &BScanSegmentation::autoAddRemoveArea);
	connect(this, &BScanSegmentation::paintAutoAreaSelected, autoRemoveAddAreaAction, &QAction::setChecked);
	toolBar->addAction(autoRemoveAddAreaAction);
	actionGroupMethod->addAction(autoRemoveAddAreaAction);

	QAction* removeAreaAction = new QAction(parent);
	removeAreaAction->setCheckable(true);
	removeAreaAction->setText(tr("Add"));
	removeAreaAction->setIcon(createMonocromeIcon(paintArea1Value*255));
	connect(removeAreaAction, &QAction::triggered, this, &BScanSegmentation::paintArea1Slot);
	connect(this, &BScanSegmentation::paintArea1Selected, removeAreaAction, &QAction::setChecked);
	toolBar->addAction(removeAreaAction);
	actionGroupMethod->addAction(removeAreaAction);

	toolBar->addSeparator();

	QAction* actionInitFromIlm = new QAction(parent);
	actionInitFromIlm->setText(tr("Init from ILM"));
	actionInitFromIlm->setIcon(QIcon(":/icons/wand.png"));
	connect(actionInitFromIlm, &QAction::triggered, this, &BScanSegmentation::initFromSegmentline);
	connect(this, &BScanSegmentation::paintArea1Selected, actionInitFromIlm, &QAction::setChecked);
	toolBar->addAction(actionInitFromIlm);
	
	
	QAction* actionInitFromTrashold = new QAction(parent);
	actionInitFromTrashold->setText(tr("Init from threshold"));
	actionInitFromTrashold->setIcon(QIcon(":/icons/wand.png"));
	connect(actionInitFromTrashold, &QAction::triggered, this, &BScanSegmentation::initFromThreshold);
	connect(this, &BScanSegmentation::paintArea1Selected, actionInitFromTrashold, &QAction::setChecked);
	toolBar->addAction(actionInitFromTrashold);
	
	toolBar->addSeparator();

	QAction* actionErodeBScan = new QAction(parent);
	actionErodeBScan->setText(tr("Erode"));
	actionErodeBScan->setIcon(QIcon(":/icons/arrow_in.png"));
	connect(actionErodeBScan, &QAction::triggered, this, &BScanSegmentation::erodeBScan);
	connect(this, &BScanSegmentation::paintArea1Selected, actionErodeBScan, &QAction::setChecked);
	toolBar->addAction(actionErodeBScan);


	QAction* actionDilateBScan = new QAction(parent);
	actionDilateBScan->setText(tr("Dilate"));
	actionDilateBScan->setIcon(QIcon(":/icons/arrow_out.png"));
	connect(actionDilateBScan, &QAction::triggered, this, &BScanSegmentation::dilateBScan);
	connect(this, &BScanSegmentation::paintArea1Selected, actionDilateBScan, &QAction::setChecked);
	toolBar->addAction(actionDilateBScan);

	QAction* actionOpenCloseBScan = new QAction(parent);
	actionOpenCloseBScan->setText(tr("Open/Close"));
	actionOpenCloseBScan->setIcon(QIcon(":/icons/arrow_inout.png"));
	connect(actionOpenCloseBScan, &QAction::triggered, this, &BScanSegmentation::opencloseBScan);
	connect(this, &BScanSegmentation::paintArea1Selected, actionOpenCloseBScan, &QAction::setChecked);
	toolBar->addAction(actionOpenCloseBScan);

	QAction* actionMedianBScan = new QAction(parent);
	actionMedianBScan->setText(tr("Median"));
	actionMedianBScan->setIcon(QIcon(":/icons/arrow_inout.png"));
	connect(actionMedianBScan, &QAction::triggered, this, &BScanSegmentation::medianBScan);
	connect(this, &BScanSegmentation::paintArea1Selected, actionMedianBScan, &QAction::setChecked);
	toolBar->addAction(actionMedianBScan);


	connectToolBar(toolBar);
	
	return toolBar;
}

bool BScanSegmentation::setOnCoord(int x, int y, int factor)
{
	if(factor == 0)
		return false;
	
	cv::Mat* map = segments.at(getActBScan());
	if(!map || map->empty())
		return false;

	switch(paintMethod)
	{
		case PaintMethod::Disc:
			cv::circle(*map, cv::Point(x/factor, y/factor), paintRadius, paintValue, CV_FILLED, 8, 0);
			break;
		case PaintMethod::Quadrat:
			rectangle(*map, cv::Point(x/factor-paintRadius, y/factor-paintRadius), cv::Point(x/factor+paintRadius, y/factor+paintRadius), paintValue, CV_FILLED, 8, 0);
			break;
	}
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

bool BScanSegmentation::keyPressEvent(QKeyEvent* e, BScanMarkerWidget*)
{
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

void BScanSegmentation::newSeriesLoaded(const OctData::Series* series, boost::property_tree::ptree& markerTree)
{
	clearSegments();
	
	if(!series)
		return;
	
	for(const OctData::BScan* bscan : series->getBScans())
	{
		cv::Mat* mat = new cv::Mat(bscan->getHeight(), bscan->getWidth(), cv::DataType<uint8_t>::type, cvScalar(initialValue));
		segments.push_back(mat);
	}
	
	BScanSegmentationPtree::parsePTree(markerTree, this);
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
	requestUpdate();
}

void BScanSegmentation::initFromThreshold()
{
	const OctData::Series* series = getSeries();
	SegMats::iterator segMatIt = segments.begin();
	
	double factor = 0.40;
	
	for(const OctData::BScan* bscan : series->getBScans())
	{
		const cv::Mat& image = bscan->getImage();
		cv::Mat* mat = *segMatIt;
		
		if(mat && !mat->empty())
		{
			internalMatType* colIt = mat->ptr<internalMatType>();
			const uint8_t* imageColIt = image.ptr<uint8_t>();
			
			std::size_t colSize = static_cast<std::size_t>(mat->cols);
			std::size_t rowSize = static_cast<std::size_t>(mat->rows);
			// TODO: size check
			
			for(std::size_t col = 0; col < colSize; ++col)
			{
				const uint8_t* imageRowIt = imageColIt;
				uint8_t maxValue = 0;
				
				for(std::size_t row = 0; row < rowSize; ++row)
				{
					if(*imageRowIt > maxValue)
						maxValue = *imageRowIt;
					imageRowIt += colSize;
				}
				
				internalMatType* rowIt = colIt;
				imageRowIt = imageColIt;
				
				uint8_t searchValue = maxValue*factor;
				std::size_t row = 0;
				std::size_t strikes = 0;
				for(; row < rowSize; ++row)
				{
					if(*imageRowIt > searchValue)
					{
						if(strikes > 6 || *imageRowIt == maxValue)
							break;
						++strikes;
					}
					else
						strikes = 0;
					
					*rowIt =  1;
					rowIt      += colSize;
					imageRowIt += colSize;
				}
				
				if(row < rowSize)
				{
					row        -= strikes;
					rowIt      -= colSize*strikes;
					imageRowIt -= colSize*strikes;
				}
				
				for(; row < rowSize; ++row)
				{
					*rowIt = 0;
					rowIt      += colSize;
					imageRowIt += colSize;
				}
				
				++colIt;
				++imageColIt;
			}
		}
		++segMatIt;
	}
	requestUpdate();
}

void BScanSegmentation::dilateBScan()
{
	cv::Mat* map = segments.at(getActBScan());
	if(!map || map->empty())
		return;

	int iterations = 1;
	cv::dilate(*map, *map, cv::Mat(), cv::Point(-1, -1), iterations, cv::BORDER_REFLECT_101, 1);

	requestUpdate();
}

void BScanSegmentation::erodeBScan()
{
	cv::Mat* map = segments.at(getActBScan());
	if(!map || map->empty())
		return;

	int iterations = 1;
	cv::erode(*map, *map, cv::Mat(), cv::Point(-1, -1), iterations, cv::BORDER_REFLECT_101, 1);

	requestUpdate();
}

void BScanSegmentation::opencloseBScan()
{
	cv::Mat* map = segments.at(getActBScan());
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
	cv::Mat* map = segments.at(getActBScan());
	if(!map || map->empty())
		return;

	medianBlur(*map, *map, 3);

	requestUpdate();
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
			mat->setTo(cv::Scalar(initialValue));
		}
	}

	BScanSegmentationPtree::parsePTree(markerTree, this);
}

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

