#include "bscanmarkerwidget.h"

#include <manager/bscanmarkermanager.h>
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

BScanMarkerWidget::BScanMarkerWidget(BScanMarkerManager& markerManger)
: CVImageWidget()
, markerManger(markerManger)
{
	
	OctDataManager& octdataManager = OctDataManager::getInstance();

	addZoomItems();

	connect(&octdataManager, &OctDataManager::seriesChanged   , this, &BScanMarkerWidget::cscanLoaded         );
	connect(&markerManger  , &BScanMarkerManager::bscanChanged, this, &BScanMarkerWidget::bscanChanged        );
	//connect(&markerManger, &BScanMarkerManager::markerMethodChanged, this, &BScanMarkerWidget::markersMethodChanged);

	connect(this, &BScanMarkerWidget::bscanChangeInkrement, &markerManger, &BScanMarkerManager::inkrementBScan);

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
	void paintSegmentationLine(QPainter& segPainter, int bScanHeight, const OctData::BScan::Segmentline& segLine, double factor)
	{
		double lastEnt = std::numeric_limits<double>::quiet_NaN();
		int xCoord = 0;
		for(double value : segLine)
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
	pen.setColor(QColor(255, 0, 0, 180));
	pen.setWidth(1);
	segPainter.setPen(pen);
	int bScanHeight = actBscan->getHeight();
	double scaleFactor = getImageScaleFactor();

	if(ProgramOptions::bscansShowSegmentationslines())
	{
		paintSegmentationLine(segPainter, bScanHeight, actBscan->getSegmentLine(OctData::BScan::SegmentlineType::ILM  ), scaleFactor);
		paintSegmentationLine(segPainter, bScanHeight, actBscan->getSegmentLine(OctData::BScan::SegmentlineType::BM   ), scaleFactor);
		paintSegmentationLine(segPainter, bScanHeight, actBscan->getSegmentLine(OctData::BScan::SegmentlineType::NFL  ), scaleFactor);
		
		paintSegmentationLine(segPainter, bScanHeight, actBscan->getSegmentLine(OctData::BScan::SegmentlineType::I3T1 ), scaleFactor);
		paintSegmentationLine(segPainter, bScanHeight, actBscan->getSegmentLine(OctData::BScan::SegmentlineType::I4T1 ), scaleFactor);
		paintSegmentationLine(segPainter, bScanHeight, actBscan->getSegmentLine(OctData::BScan::SegmentlineType::I5T1 ), scaleFactor);
		paintSegmentationLine(segPainter, bScanHeight, actBscan->getSegmentLine(OctData::BScan::SegmentlineType::I6T1 ), scaleFactor);
		paintSegmentationLine(segPainter, bScanHeight, actBscan->getSegmentLine(OctData::BScan::SegmentlineType::I8T3 ), scaleFactor);
		paintSegmentationLine(segPainter, bScanHeight, actBscan->getSegmentLine(OctData::BScan::SegmentlineType::I14T1), scaleFactor);
		paintSegmentationLine(segPainter, bScanHeight, actBscan->getSegmentLine(OctData::BScan::SegmentlineType::I15T1), scaleFactor);
		paintSegmentationLine(segPainter, bScanHeight, actBscan->getSegmentLine(OctData::BScan::SegmentlineType::I16T1), scaleFactor);
	}
	
	
	QPainter painter(this);
	
	BscanMarkerBase* actMarker = markerManger.getActMarker();
	if(actMarker)
		actMarker->drawMarker(painter, this);

/*

	for(const BScanMarkerManager::MarkerMap::interval_mapping_type pair : markerManger.getMarkers())
	{
		IntervallMarker::Marker marker = pair.second;
		if(marker.isDefined())
		{
			boost::icl::discrete_interval<int> itv  = pair.first;
			painter.fillRect(itv.lower()*scaleFactor, 0, (itv.upper()-itv.lower())*scaleFactor, height(), QColor(marker.getRed(), marker.getGreen(), marker.getBlue(),  60));
		}
	}
	

	if(mouseInWidget && markerManger.getMarkerMethod() == BScanMarkerManager::Method::Paint)
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

	*/
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

	BscanMarkerBase* actMarker = markerManger.getActMarker();
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
	
	BscanMarkerBase* actMarker = markerManger.getActMarker();
	if(actMarker)
		if(actMarker->mouseMoveEvent(event, this))
			update();

}

void BScanMarkerWidget::mousePressEvent(QMouseEvent* event)
{
	QWidget::mousePressEvent(event);

	
	BscanMarkerBase* actMarker = markerManger.getActMarker();
	if(actMarker)
		if(actMarker->mousePressEvent(event, this))
			update();
	
}

void BScanMarkerWidget::mouseReleaseEvent(QMouseEvent* event)
{
	QWidget::mouseReleaseEvent(event);
	
	BscanMarkerBase* actMarker = markerManger.getActMarker();
	if(actMarker)
		if(actMarker->mouseReleaseEvent(event, this))
			update();
	
}

void BScanMarkerWidget::keyPressEvent(QKeyEvent* e)
{
	QWidget::keyPressEvent(e);
	
	switch(e->key())
	{
		case Qt::Key_Left:
			emit(bscanChangeInkrement(-1));
			break;
		case Qt::Key_Right:
			emit(bscanChangeInkrement( 1));
			break;
		default:
			BscanMarkerBase* actMarker = markerManger.getActMarker();
			if(actMarker)
				if(actMarker->keyPressEvent(e, this))
					update();
			// TODO: weiterleiten an marker
			break;
	}
	
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

