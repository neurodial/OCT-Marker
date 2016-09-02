#include "bscanmarkerwidget.h"

#include <manager/markermanager.h>
#include <data_structure/intervallmarker.h>
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

BScanMarkerWidget::BScanMarkerWidget(MarkerManager& markerManger)
: CVImageWidget()
, markerManger(markerManger)
{

	connect(&markerManger, &MarkerManager::newCScanLoaded     , this, &BScanMarkerWidget::cscanLoaded         );
	connect(&markerManger, &MarkerManager::bscanChanged       , this, &BScanMarkerWidget::bscanChanged        );
	connect(&markerManger, &MarkerManager::markerMethodChanged, this, &BScanMarkerWidget::markersMethodChanged);

	connect(this, &BScanMarkerWidget::bscanChangeInkrement, &markerManger, &MarkerManager::inkrementBScan);

	connect(&ProgramOptions::bscansShowSegmentationslines, &OptionBool::valueChanged, this, &BScanMarkerWidget::viewOptionsChangedSlot);
	
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

namespace
{
	void paintSegmentationLine(QPainter& segPainter, int bScanHeight, const OctData::BScan::Segmentline& segLine)
	{
		double lastEnt = std::numeric_limits<double>::quiet_NaN();
		int xCoord = 0;
		for(double value : segLine)
		{
			// std::cout << value << '\n';
			if(!std::isnan(lastEnt) && lastEnt < bScanHeight && lastEnt > 0 && value < bScanHeight && value > 0)
			{
				segPainter.drawLine(QLineF(xCoord-1, lastEnt, xCoord, value));
			}
			lastEnt = value;
			++xCoord;
		}
	}
}

void BScanMarkerWidget::paintEvent(QPaintEvent* event)
{
	CVImageWidget::paintEvent(event);

	if(!markerManger.cscanLoaded())
		return;


	QPainter segPainter(this);
	QPen pen;
	pen.setColor(QColor(255, 0, 0, 180));
	pen.setWidth(1);
	segPainter.setPen(pen);
	int bScanHeight = actBscan->getHeight();

	if(ProgramOptions::bscansShowSegmentationslines())
	{
		paintSegmentationLine(segPainter, bScanHeight, actBscan->getSegmentLine(OctData::BScan::SegmentlineType::ILM));
		paintSegmentationLine(segPainter, bScanHeight, actBscan->getSegmentLine(OctData::BScan::SegmentlineType::BM));
		paintSegmentationLine(segPainter, bScanHeight, actBscan->getSegmentLine(OctData::BScan::SegmentlineType::NFL));
	}


	QPainter painter(this);

	for(const MarkerManager::MarkerMap::interval_mapping_type pair : markerManger.getMarkers())
	{
		IntervallMarker::Marker marker = pair.second;
		if(marker.isDefined())
		{
			boost::icl::discrete_interval<int> itv  = pair.first;
			painter.fillRect(itv.lower(), 0, itv.upper()-itv.lower(), height(), QColor(marker.getRed(), marker.getGreen(), marker.getBlue(),  60));
		}
	}
	

	if(mouseInWidget && markerManger.getMarkerMethod() == MarkerManager::Method::Paint)
	{
		painter.drawLine(mousePos.x(), 0, mousePos.x(), height());

		if(markerActiv)
		{
			const IntervallMarker::Marker& marker = markerManger.getActMarker();
			if(mousePos.x() != clickPos.x())
			{
				painter.drawLine(clickPos.x(), 0, clickPos.x(), height());
				QPen pen;
				pen.setColor(QColor(marker.getRed(), marker.getGreen(), marker.getBlue(), 255));
				pen.setWidth(5);
				painter.setPen(pen);
				painter.drawLine(mousePos, clickPos);
			}
		}
	}

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
	const OctData::Series& series = markerManger.getSeries();
	actBscan = series.getBScan(bscanNR);
	
	updateRawExport();
	if(actBscan)
		showImage(actBscan->getImage());
	else
		update();
}

void BScanMarkerWidget::leaveEvent(QEvent* e)
{
	QWidget::leaveEvent(e);

	mouseInWidget = false;
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



void BScanMarkerWidget::wheelEvent(QWheelEvent* wheelE)
{
	int deltaWheel = wheelE->delta();
	if(deltaWheel < 0)
		emit(bscanChangeInkrement(-1));
	else
		emit(bscanChangeInkrement(+1));
}


void BScanMarkerWidget::mouseMoveEvent(QMouseEvent* event)
{
	QWidget::mouseMoveEvent(event);
	
	mouseInWidget = true;
	// if(markerActiv)
	{
		mousePos = event->pos();
		update();
	}
}

void BScanMarkerWidget::mousePressEvent(QMouseEvent* event)
{
	QWidget::mousePressEvent(event);

	if(event->button() == Qt::LeftButton)
	{
		clickPos = event->pos();
		mousePos = event->pos();
		markerActiv = true;
	}
	else
		markerActiv = false;
		
}

void BScanMarkerWidget::mouseReleaseEvent(QMouseEvent* event)
{
	QWidget::mouseReleaseEvent(event);

	switch(markerManger.getMarkerMethod())
	{
		case MarkerManager::Method::Paint:
			if(clickPos.x() != event->x() && markerActiv)
			{
				// std::cout << __FUNCTION__ << ": " << clickPos << " - " << event->x() << std::endl;
				markerManger.setMarker(clickPos.x(), event->x());
			}
			break;
		case MarkerManager::Method::Fill:
			if(markerActiv)
				markerManger.fillMarker(clickPos.x());
			break;
	}
	
	markerActiv = false;
	repaint();
}

void BScanMarkerWidget::keyPressEvent(QKeyEvent* e)
{
	QWidget::keyPressEvent(e);
	
	switch(e->key())
	{
		case Qt::Key_Escape:
			markerActiv = false;
			repaint();
			break;
		case Qt::Key_Left:
			emit(bscanChangeInkrement(-1));
			break;
		case Qt::Key_Right:
			emit(bscanChangeInkrement( 1));
			break;
	}
	
}


void BScanMarkerWidget::markersMethodChanged()
{
	switch(markerManger.getMarkerMethod())
	{
		case MarkerManager::Method::Paint:
			setMouseTracking(true);
			break;
		default:
			setMouseTracking(false);
			break;
	}
	update();
}

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
