#include "bscanmarkerwidget.h"

#include <manager/octmarkermanager.h>
#include <manager/octdatamanager.h>

#include <markermodules/bscanmarkerbase.h>

#include <data_structure/intervalmarker.h>
#include <data_structure/programoptions.h>

#include <octdata/datastruct/series.h>
#include <octdata/datastruct/bscan.h>

#include <cpp_framework/cvmat/treestructbin.h>

#include <cmath>

#include <QWheelEvent>
#include <QPainter>
#include <QMenu>
#include <QMessageBox>
#include <QFileDialog>

namespace
{
	inline bool modPressed(QKeyEvent* e)
	{
		return e->modifiers() & Qt::ControlModifier;
	}
	inline bool modPressed(QMouseEvent* e)
	{
		return e->modifiers() & Qt::ControlModifier;
	}
}

BScanMarkerWidget::BScanMarkerWidget()
: CVImageWidget()
, markerManger(OctMarkerManager::getInstance())
{
	
	OctDataManager& octdataManager = OctDataManager::getInstance();

	addZoomItems();

	connect(&octdataManager, &OctDataManager::seriesChanged , this, &BScanMarkerWidget::cscanLoaded         );
	connect(&markerManger  , &OctMarkerManager::bscanChanged, this, &BScanMarkerWidget::bscanChanged        );
	//connect(&markerManger, &BScanMarkerManager::markerMethodChanged, this, &BScanMarkerWidget::markersMethodChanged);

	connect(this, &BScanMarkerWidget::bscanChangeInkrement, &markerManger, &OctMarkerManager::inkrementBScan);

	connect(&ProgramOptions::bscansShowSegmentationslines, &OptionBool ::valueChanged, this, &BScanMarkerWidget::viewOptionsChangedSlot);
	connect(&ProgramOptions::bscanSegmetationLineColor   , &OptionColor::valueChanged, this, &BScanMarkerWidget::viewOptionsChangedSlot);
	connect(&ProgramOptions::bscanSegmetationLineThicknes, &OptionInt  ::valueChanged, this, &BScanMarkerWidget::viewOptionsChangedSlot);
	
	setFocusPolicy(Qt::ClickFocus);
	setMouseTracking(true);
	
	contextMenu->addSeparator();
	
	saveRawImageAction = new QAction(this);
	saveRawImageAction->setText(tr("Save Raw Image"));
	saveRawImageAction->setIcon(QIcon(":/icons/disk.png"));
	contextMenu->addAction(saveRawImageAction);
	connect(saveRawImageAction, SIGNAL(triggered(bool)), this, SLOT(saveRawImage()));
	
	
	saveRawMatAction = new QAction(this);
	saveRawMatAction->setText(tr("Save raw data as matrix"));
	saveRawMatAction->setIcon(QIcon(":/icons/disk.png"));
	contextMenu->addAction(saveRawMatAction);
	connect(saveRawMatAction, SIGNAL(triggered(bool)), this, SLOT(saveRawMat()));
	

	contextMenu->addSeparator();
	
	saveRawBinAction = new QAction(this);
	saveRawBinAction->setText(tr("Save raw data as bin"));
	saveRawBinAction->setIcon(QIcon(":/icons/disk.png"));
	contextMenu->addAction(saveRawBinAction);
	connect(saveRawBinAction, SIGNAL(triggered(bool)), this, SLOT(saveRawBin()));
	
	saveImageBinAction = new QAction(this);
	saveImageBinAction->setText(tr("Save image data as bin"));
	saveImageBinAction->setIcon(QIcon(":/icons/disk.png"));
	contextMenu->addAction(saveImageBinAction);
	connect(saveImageBinAction, SIGNAL(triggered(bool)), this, SLOT(saveImageBin()));
}


BScanMarkerWidget::~BScanMarkerWidget()
{
}

void BScanMarkerWidget::paintSegmentationLine(QPainter& segPainter, int bScanHeight, const std::vector<double>& segLine, double factor)
{
	double lastEnt = std::numeric_limits<OctData::Segmentationlines::SegmentlineDataType>::quiet_NaN();
	int xCoord = 0;
	for(OctData::Segmentationlines::SegmentlineDataType value : segLine)
	{
		// std::cout << value << '\n';
		if(!std::isnan(lastEnt) && lastEnt < bScanHeight && lastEnt > 0 && value < bScanHeight && value > 0)
		{
			segPainter.drawLine(QLineF((xCoord-1)*factor, lastEnt*factor, xCoord*factor, value*factor));
		}
		lastEnt = value;
		++xCoord;
	}
}



void BScanMarkerWidget::paintEvent(QPaintEvent* event)
{
	CVImageWidget::paintEvent(event);

	
	OctDataManager& octdataManager = OctDataManager::getInstance();
	const OctData::Series* series = octdataManager.getSeries();
	
	if(!series)
		return;

	if(!actBscan)
		return;

	QPainter segPainter(this);
	QPen pen;
	pen.setColor(ProgramOptions::bscanSegmetationLineColor());
	pen.setWidth(ProgramOptions::bscanSegmetationLineThicknes());
	segPainter.setPen(pen);
	int bScanHeight = actBscan->getHeight();
	double scaleFactor = getImageScaleFactor();

	if(ProgramOptions::bscansShowSegmentationslines())
	{

		for(OctData::Segmentationlines::SegmentlineType type : OctData::Segmentationlines::getSegmentlineTypes())
			BScanMarkerWidget::paintSegmentationLine(segPainter, bScanHeight, actBscan->getSegmentLine(type), scaleFactor);
		/*
		paintSegmentationLine(segPainter, bScanHeight, actBscan->getSegmentLine(OctData::Segmentationlines::SegmentlineType::ILM  ), scaleFactor);
		paintSegmentationLine(segPainter, bScanHeight, actBscan->getSegmentLine(OctData::Segmentationlines::SegmentlineType::BM   ), scaleFactor);
		paintSegmentationLine(segPainter, bScanHeight, actBscan->getSegmentLine(OctData::Segmentationlines::SegmentlineType::NFL  ), scaleFactor);

		paintSegmentationLine(segPainter, bScanHeight, actBscan->getSegmentLine(OctData::Segmentationlines::SegmentlineType::I3T1 ), scaleFactor);
		paintSegmentationLine(segPainter, bScanHeight, actBscan->getSegmentLine(OctData::Segmentationlines::SegmentlineType::I4T1 ), scaleFactor);
		paintSegmentationLine(segPainter, bScanHeight, actBscan->getSegmentLine(OctData::Segmentationlines::SegmentlineType::I5T1 ), scaleFactor);
		paintSegmentationLine(segPainter, bScanHeight, actBscan->getSegmentLine(OctData::Segmentationlines::SegmentlineType::I6T1 ), scaleFactor);
		paintSegmentationLine(segPainter, bScanHeight, actBscan->getSegmentLine(OctData::Segmentationlines::SegmentlineType::I8T3 ), scaleFactor);
		paintSegmentationLine(segPainter, bScanHeight, actBscan->getSegmentLine(OctData::Segmentationlines::SegmentlineType::I14T1), scaleFactor);
		paintSegmentationLine(segPainter, bScanHeight, actBscan->getSegmentLine(OctData::Segmentationlines::SegmentlineType::I15T1), scaleFactor);
		paintSegmentationLine(segPainter, bScanHeight, actBscan->getSegmentLine(OctData::Segmentationlines::SegmentlineType::I16T1), scaleFactor);*/
	}
	
	
	QPainter painter(this);
	
	BscanMarkerBase* actMarker = markerManger.getActBscanMarker();
	if(actMarker)
		actMarker->drawMarker(painter, this, event->rect());

	painter.end();
}

bool BScanMarkerWidget::existsRaw() const
{
	if(!actBscan)
		return false;
	return !(actBscan->getRawImage().empty());
}

bool BScanMarkerWidget::rawSaveableAsImage() const
{
	if(!actBscan)
		return false;
	cv::Mat rawImage = actBscan->getRawImage();
	if(existsRaw())
	{
		int chanels = rawImage.channels();
		if(rawImage.depth() == CV_8U || rawImage.depth() == CV_16U)
			if(chanels == 1 || chanels == 3)
				return true;
	}
	return false;
}


void BScanMarkerWidget::updateRawExport()
{
	bool rawExists       = existsRaw();
	bool saveableAsImage = rawSaveableAsImage();

	
	saveRawImageAction->setEnabled(saveableAsImage);
	saveRawMatAction  ->setEnabled(rawExists);
	saveRawBinAction  ->setEnabled(rawExists);
}


void BScanMarkerWidget::bscanChanged(int bscanNR)
{
	const OctData::Series* series = OctDataManager::getInstance().getSeries();
	if(!series)
		return;
	
	actBscan = series->getBScan(bscanNR);
	
	updateRawExport();
	if(actBscan)
		showImage(actBscan->getImage());
	else
		update();
}

void BScanMarkerWidget::leaveEvent(QEvent* event)
{
	QWidget::leaveEvent(event);

	mouseLeaveImage();

	BscanMarkerBase* actMarker = markerManger.getActBscanMarker();
	if(actMarker)
		if(actMarker->leaveWidgetEvent(event, this))
			update();
}


void BScanMarkerWidget::cscanLoaded()
{
	bscanChanged(markerManger.getActBScan());
}

void BScanMarkerWidget::viewOptionsChangedSlot()
{
	update();
}


bool BScanMarkerWidget::event(QEvent* event)
{
	if(event->type() == QEvent::ToolTip)
	{
		BscanMarkerBase* actMarker = markerManger.getActBscanMarker();
		if(actMarker)
		{
			if(!actMarker->toolTipEvent(event, this))
				event->ignore();
		}
		return true;
	}
	return QWidget::event(event);
}



void BScanMarkerWidget::wheelEvent(QWheelEvent* wheelE)
{
	int deltaWheel = wheelE->delta();
	if(deltaWheel < 0)
		emit(bscanChangeInkrement(-1));
	else
		emit(bscanChangeInkrement(+1));

	wheelE->accept();
}


void BScanMarkerWidget::mouseMoveEvent(QMouseEvent* event)
{
	QWidget::mouseMoveEvent(event);

	int xImg, yImg;
	transformCoordWidget2Img(event->x(), event->y(), xImg, yImg);
	mousePosInImage(xImg, yImg);

// 	if(checkControlUsed(event))
// 		return;

	BscanMarkerBase* actMarker = markerManger.getActBscanMarker();
	if(actMarker)
	{
		BscanMarkerBase::RedrawRequest result = actMarker->mouseMoveEvent(event, this);
		if(result.redraw)
		{
			if(result.rect.isValid())
				update(result.rect);
			else
				update();
		}
	}

	if(!checkControlUsed(event) && actMarker)
		event->accept();
}

void BScanMarkerWidget::mousePressEvent(QMouseEvent* event)
{
	QWidget::mousePressEvent(event);

	// TODO: workaround for pan in scrollarea
	if(checkControlUsed(event))
		return;

	
	BscanMarkerBase* actMarker = markerManger.getActBscanMarker();
	if(actMarker)
	{
		BscanMarkerBase::RedrawRequest result = actMarker->mousePressEvent(event, this);
		if(result.redraw)
		{
			if(result.rect.isValid())
				update(result.rect);
			else
				update();
		}
		event->accept();
	}
	
}

void BScanMarkerWidget::mouseReleaseEvent(QMouseEvent* event)
{
	QWidget::mouseReleaseEvent(event);

	if(checkControlUsed(event))
		return;
	
	BscanMarkerBase* actMarker = markerManger.getActBscanMarker();
	if(actMarker)
	{
		BscanMarkerBase::RedrawRequest result = actMarker->mouseReleaseEvent(event, this);
		if(result.redraw)
		{
			if(result.rect.isValid())
				update(result.rect);
			else
				update();
		}
		event->accept();
	}
}

void BScanMarkerWidget::keyPressEvent(QKeyEvent* e)
{
	QWidget::keyPressEvent(e);

	checkControlUsed(e);

	switch(e->key())
	{
		case Qt::Key_Left:
			emit(bscanChangeInkrement(-1));
			e->accept();
			break;
		case Qt::Key_Right:
			emit(bscanChangeInkrement( 1));
			e->accept();
			break;
		default:
			BscanMarkerBase* actMarker = markerManger.getActBscanMarker();
			if(actMarker)
				if(actMarker->keyPressEvent(e, this))
					update();
			break;
	}
}

void BScanMarkerWidget::keyReleaseEvent(QKeyEvent* e)
{
	QWidget::keyReleaseEvent(e);
	checkControlUsed(e);
}


bool BScanMarkerWidget::checkControlUsed(QMouseEvent* event)
{
	return checkControlUsed(modPressed(event));
}

bool BScanMarkerWidget::checkControlUsed(QKeyEvent* event)
{
	return checkControlUsed(modPressed(event));
}


inline bool BScanMarkerWidget::checkControlUsed(bool modPressed)
{
	if(modPressed)
	{
		if(!controlUsed)
		{
			BscanMarkerBase* actMarker = markerManger.getActBscanMarker();
			if(actMarker)
				if(actMarker->setMarkerActive(false, this))
					update();
		}
		controlUsed = true;

		return true;
	}

	bool result = controlUsed;

	if(controlUsed)
	{
		BscanMarkerBase* actMarker = markerManger.getActBscanMarker();
		if(actMarker)
			if(actMarker->setMarkerActive(true, this))
				update();

		controlUsed = false;
	}

	return result;
}


/*
void BScanMarkerWidget::markersMethodChanged()
{
	switch(markerManger.getMarkerMethod())
	{
		case BScanMarkerManager::Method::Paint:
			setMouseTracking(true);
			break;
		default:
			setMouseTracking(false);
			break;
	}
	update();
}
*/

void BScanMarkerWidget::saveRawImage()
{
	if(actBscan && rawSaveableAsImage())
	{
		QString filename;
		if(fdSaveRaw(filename))
		{
			if(!cv::imwrite(filename.toStdString(), actBscan->getRawImage()))
			{
				QMessageBox msgBox;
				msgBox.setText("image not saved");
				msgBox.setIcon(QMessageBox::Critical);
				msgBox.exec();
			}
		}
	}
}

void BScanMarkerWidget::saveRawMat()
{
	
	if(actBscan && existsRaw())
	{
		QString filename = QFileDialog::getSaveFileName(this, tr("Save raw data as matrix"), "", "CV (*.xml *.jml)");
		if(!filename.isEmpty())
		{
			cv::FileStorage fs(filename.toStdString(), cv::FileStorage::WRITE);
			fs << "BScan" << actBscan->getRawImage();
		}
	}
}


void BScanMarkerWidget::saveRawBin()
{
	if(actBscan && existsRaw())
	{
		QString filename = QFileDialog::getSaveFileName(this, tr("Save raw data as bin"), "", "Binary (*.bin)");
		if(!filename.isEmpty())
		{
			CppFW::CVMatTreeStructBin::writeBin(filename.toStdString(), actBscan->getRawImage());
		}
	}
}

void BScanMarkerWidget::saveImageBin()
{
	if(actBscan)
	{
		QString filename = QFileDialog::getSaveFileName(this, tr("Save image data as bin"), "", "Binary (*.bin)");
		if(!filename.isEmpty())
		{
			CppFW::CVMatTreeStructBin::writeBin(filename.toStdString(), actBscan->getImage());
		}
	}
}

int BScanMarkerWidget::fdSaveRaw(QString& filename)
{
	QFileDialog fd(this);
	// fd.selectFile(lineEditFile->text());
	fd.setWindowTitle(tr("Choose a filename to save under"));
	fd.setAcceptMode(QFileDialog::AcceptSave);
	
	QStringList mimeTypeFilters;
	mimeTypeFilters << "image/png" << "image/tiff";
	fd.setMimeTypeFilters(mimeTypeFilters);
	fd.setFileMode(QFileDialog::AnyFile);
	
	int result = fd.exec();
	
	if(result)
	{
		filename = fd.selectedFiles().first();
	}
	return result;
}

