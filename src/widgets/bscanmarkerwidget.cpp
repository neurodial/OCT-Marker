#include "bscanmarkerwidget.h"

#include<fstream>
#include<cmath>

#include<QWheelEvent>
#include<QPainter>
#include<QMenu>
#include<QMessageBox>
#include<QFileDialog>
#include<QGraphicsView>

#include<cpp_framework/cvmat/treestructbin.h>

#include<octdata/datastruct/series.h>
#include<octdata/datastruct/bscan.h>

#include<manager/octmarkermanager.h>
#include<manager/octdatamanager.h>
#include<manager/paintmarker.h>

#include<markermodules/bscanmarkerbase.h>

#include<data_structure/intervalmarker.h>
#include<data_structure/programoptions.h>
#include<data_structure/extraseriesdata.h>
#include<data_structure/conturesegment.h>


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

	class GraphicsView : public QGraphicsView
	{
		double imageSizeX = 1000;
		double imageSizeY = 1000;

		void updateSize()
		{
			setSceneRect    (0, 0, imageSizeX, imageSizeY);
			fitInView(QRectF(0, 0, imageSizeX, imageSizeY));
		}
	public:
		explicit GraphicsView(QWidget* parent) : QGraphicsView(parent) {}

		void setImageSize(double x, double y) { imageSizeX = x; imageSizeY = y; updateSize(); }
	protected:
		virtual void resizeEvent(QResizeEvent *event) override
		{
			QGraphicsView::resizeEvent(event);
			updateSize();
		}
	};

}

BScanMarkerWidget::BScanMarkerWidget()
: CVImageWidget()
, markerManger(OctMarkerManager::getInstance())
{
	
	OctDataManager& octdataManager = OctDataManager::getInstance();

	addZoomItems();

	connect(&octdataManager, &OctDataManager::seriesChanged       , this, &BScanMarkerWidget::cscanLoaded         );
	connect(&markerManger  , &OctMarkerManager::bscanChanged      , this, &BScanMarkerWidget::imageChanged        );
	connect(&markerManger  , &OctMarkerManager::bscanMarkerChanged, this, &BScanMarkerWidget::markersMethodChanged);

	connect(this, &BScanMarkerWidget::bscanChangeInkrement, &markerManger, &OctMarkerManager::inkrementBScan);

	connect(&ProgramOptions::bscansShowSegmentationslines    , &OptionBool ::valueChanged, this, &BScanMarkerWidget::viewOptionsChangedSlot);
	connect(&ProgramOptions::bscanSegmetationLineColor       , &OptionColor::valueChanged, this, &BScanMarkerWidget::viewOptionsChangedSlot);
	connect(&ProgramOptions::bscanSegmetationLineThicknes    , &OptionInt  ::valueChanged, this, &BScanMarkerWidget::viewOptionsChangedSlot);
	connect(&ProgramOptions::bscanShowExtraSegmentationslines, &OptionBool ::valueChanged, this, &BScanMarkerWidget::viewOptionsChangedSlot);

	connect(&ProgramOptions::bscanRespectAspectRatio         , &OptionBool ::valueChanged, this, &CVImageWidget    ::setUseAspectRatio     );
	connect(&ProgramOptions::bscanAutoFitImage               , &OptionBool ::trueSignal  , this, &BScanMarkerWidget::triggerAutoImageFit   );

	setUseAspectRatio(ProgramOptions::bscanRespectAspectRatio());


	setFocusPolicy(Qt::ClickFocus);
	setMouseTracking(true);


	saveRawImageAction = new QAction(this);
	saveRawImageAction->setText(tr("Save LaTeX image"));
	saveRawImageAction->setIcon(QIcon(":/icons/disk.png"));
	contextMenu->addAction(saveRawImageAction);
	connect(saveRawImageAction, &QAction::triggered, this, &BScanMarkerWidget::saveLatexImage);

	contextMenu->addSeparator();

	saveRawImageAction = new QAction(this);
	saveRawImageAction->setText(tr("Save Raw Image"));
	saveRawImageAction->setIcon(QIcon(":/icons/disk.png"));
	contextMenu->addAction(saveRawImageAction);
	connect(saveRawImageAction, &QAction::triggered, this, &BScanMarkerWidget::saveRawImage);


	saveRawMatAction = new QAction(this);
	saveRawMatAction->setText(tr("Save raw data as matrix"));
	saveRawMatAction->setIcon(QIcon(":/icons/disk.png"));
	contextMenu->addAction(saveRawMatAction);
	connect(saveRawMatAction, &QAction::triggered, this, &BScanMarkerWidget::saveRawMat);


	contextMenu->addSeparator();

	saveRawBinAction = new QAction(this);
	saveRawBinAction->setText(tr("Save raw data as bin"));
	saveRawBinAction->setIcon(QIcon(":/icons/disk.png"));
	contextMenu->addAction(saveRawBinAction);
	connect(saveRawBinAction, &QAction::triggered, this, &BScanMarkerWidget::saveRawBin);

	saveImageBinAction = new QAction(this);
	saveImageBinAction->setText(tr("Save image data as bin"));
	saveImageBinAction->setIcon(QIcon(":/icons/disk.png"));
	contextMenu->addAction(saveImageBinAction);
	connect(saveImageBinAction, &QAction::triggered, this, &BScanMarkerWidget::saveImageBin);

	gv = new GraphicsView(this);
	gv->setStyleSheet("QGraphicsView { border-style: none; background: transparent;}" );
	gv->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	gv->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	gv->setWindowFlags (Qt::FramelessWindowHint);
	gv->setCacheMode(QGraphicsView::CacheBackground);
	gv->setFocusPolicy(Qt::NoFocus);
	gv->setDragMode(QGraphicsView::NoDrag);

	connect(this, &CVImageWidget::sizeChanged, this, &BScanMarkerWidget::updateGraphicsViewSize);
	updateGraphicsViewSize();
	markersMethodChanged(markerManger.getActBscanMarker());
}


BScanMarkerWidget::~BScanMarkerWidget()
{
}

void BScanMarkerWidget::paintSegmentationLine(QPainter& segPainter, int bScanHeight, const std::vector<double>& segLine, const ScaleFactor& factor)
{
	double lastEnt = std::numeric_limits<OctData::Segmentationlines::SegmentlineDataType>::quiet_NaN();
	int xCoord = 0;

	const double factorX = factor.getFactorX();
	const double factorY = factor.getFactorY();

	for(OctData::Segmentationlines::SegmentlineDataType value : segLine)
	{
		// std::cout << value << '\n';
		if(!std::isnan(lastEnt) && lastEnt < bScanHeight && lastEnt > 0 && value < bScanHeight && value > 0)
		{
			segPainter.drawLine(QLineF((xCoord-1)*factorX, lastEnt*factorY, xCoord*factorX, value*factorY));
		}
		lastEnt = value;
		++xCoord;
	}
}


void BScanMarkerWidget::paintSegmentations(QPainter& segPainter, const ScaleFactor& scaleFactor) const
{
	const OctData::BScan * actBScan = markerManger.getActBScan();

	QPen pen;
	pen.setColor(ProgramOptions::bscanSegmetationLineColor());
	pen.setWidth(ProgramOptions::bscanSegmetationLineThicknes());
	segPainter.setPen(pen);
	int bScanHeight = actBScan->getHeight();

	if(ProgramOptions::bscansShowSegmentationslines())
	{

		for(OctData::Segmentationlines::SegmentlineType type : OctData::Segmentationlines::getSegmentlineTypes())
			BScanMarkerWidget::paintSegmentationLine(segPainter, bScanHeight, actBScan->getSegmentLine(type), scaleFactor);
		/*
		paintSegmentationLine(segPainter, bScanHeight, actBscan->getSegmentLine(OctData::Segmentationlines::SegmentlineType::ILM  ), scaleFactor);
		paintSegmentationLine(segPainter, bScanHeight, actBscan->getSegmentLine(OctData::Segmentationlines::SegmentlineType::BM   ), scaleFactor);
		paintSegmentationLine(segPainter, bScanHeight, actBscan->getSegmentLine(OctData::Segmentationlines::SegmentlineType::NFL  ), scaleFactor);
		*/
	}


	const ExtraImageData* extraData = markerManger.getExtraImageData();
	if(extraData)
	{
		if(ProgramOptions::bscanShowExtraSegmentationslines())
			paintConture(segPainter, extraData->getContourSegments());
	}
}



void BScanMarkerWidget::paintEvent(QPaintEvent* event)
{
	CVImageWidget::paintEvent(event);

	
	OctDataManager& octdataManager = OctDataManager::getInstance();
	const OctData::Series* series   = octdataManager.getSeries();
	const OctData::BScan * actBScan = markerManger  .getActBScan();
	
	if(!series || !actBScan)
		return;

	QPainter segPainter(this);
	paintSegmentations(segPainter, getImageScaleFactor());
	
	if(paintMarker)
		paintMarker->paintMarker(event, this);
}

void BScanMarkerWidget::paintConture(QPainter& painter, const std::vector<ContureSegment>& contours) const
{
	const ScaleFactor& scaleFactor = getImageScaleFactor();

	double scaleFactorX = scaleFactor.getFactorX();
	double scaleFactorY = scaleFactor.getFactorY();

	for(const ContureSegment& segment : contours)
	{
		if(segment.points.size() < 2)
			continue;

		Point2D lastPoint;
		if(segment.cirled)
			lastPoint = segment.points[segment.points.size() - 1];
		else
			lastPoint = segment.points[0];

		for(Point2D p : segment.points)
		{
			painter.drawLine(static_cast<int>((p        .getX()+0.5)*scaleFactorX)
			               , static_cast<int>((p        .getY()+0.5)*scaleFactorY)
			               , static_cast<int>((lastPoint.getX()+0.5)*scaleFactorX)
			               , static_cast<int>((lastPoint.getY()+0.5)*scaleFactorY));
			lastPoint = p;
		}
	}
}


bool BScanMarkerWidget::existsRaw(const OctData::BScan* bscan) const
{
	if(!bscan)
		return false;
	return !(bscan->getRawImage().empty());
}

bool BScanMarkerWidget::rawSaveableAsImage() const
{
	const OctData::BScan* actBScan = markerManger.getActBScan();
	if(!actBScan)
		return false;
	cv::Mat rawImage = actBScan->getRawImage();
	if(existsRaw(actBScan))
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
	const OctData::BScan* actBScan = markerManger.getActBScan();

	bool rawExists       = existsRaw(actBScan);
	bool saveableAsImage = rawSaveableAsImage();

	
	saveRawImageAction->setEnabled(saveableAsImage);
	saveRawMatAction  ->setEnabled(rawExists);
	saveRawBinAction  ->setEnabled(rawExists);
}


void BScanMarkerWidget::imageChanged()
{
	const OctData::BScan* actBScan = markerManger.getActBScan();

	updateRawExport();
	if(actBScan)
	{
		const OctData::ScaleFactor& sf = actBScan->getScaleFactor();

		if(sf.getX() > 0 && sf.getZ() > 0)
		{
			const double aspectRatio = sf.getZ()/sf.getX();
			setAspectRatio(aspectRatio);
		}
		else
			setAspectRatio(1.0);

		showImage(actBScan->getImage());
	}
	else
		update();
}

void BScanMarkerWidget::leaveEvent(QEvent* event)
{
	QWidget::leaveEvent(event);

	mouseLeaveImage();
	mousePosOnBScan(-1);

	BscanMarkerBase* actMarker = markerManger.getActBscanMarker();
	if(actMarker)
		if(actMarker->leaveWidgetEvent(event, this))
			update();
}


void BScanMarkerWidget::cscanLoaded()
{
	imageChanged();
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
	CVImageWidget::wheelEvent(wheelE);
	int deltaWheel = wheelE->delta();
	if(!wheelE->isAccepted())
	{
		if(deltaWheel < 0)
			emit(bscanChangeInkrement(-1));
		else
			emit(bscanChangeInkrement(+1));

		mousePosOnBScan(static_cast<double>(wheelE->x())/scaledImageWidth());
	}

	wheelE->accept();
}


void BScanMarkerWidget::contextMenuEvent(QContextMenuEvent* event)
{
	event->ignore();
	BscanMarkerBase* actMarker = markerManger.getActBscanMarker();
	if(actMarker)
		actMarker->contextMenuEvent(event);
	if(!event->isAccepted())
		CVImageWidget::contextMenuEvent(event);
}


void BScanMarkerWidget::mouseMoveEvent(QMouseEvent* event)
{
	QWidget::mouseMoveEvent(event);


	int xImg, yImg;
	transformCoordWidget2Img(event->x(), event->y(), xImg, yImg);
	mousePosInImage(xImg, yImg);
	mousePosOnBScan(static_cast<double>(event->x())/scaledImageWidth());

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

	if(actMarker && !checkControlUsed(event))
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

// 	checkControlUsed(e);

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
// 	checkControlUsed(e);
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



void BScanMarkerWidget::markersMethodChanged(BscanMarkerBase* marker)
{
	if(marker)
	{
		scene = marker->getGraphicsScene();
// 		GraphicsView* gvConvert = dynamic_cast<GraphicsView*>(gv);
// 		if(gvConvert)
// 			gvConvert->setScaleFactor(marker->getScaleFactor());
	}
	else
	{
		scene = nullptr;
	}
	gv->setScene(scene);
	gv->setVisible(scene != nullptr);
	updateGraphicsViewSize();
}


void BScanMarkerWidget::saveRawImage()
{
	const OctData::BScan* actBScan = markerManger.getActBScan();
	if(actBScan && rawSaveableAsImage())
	{
		QString filename;
		if(fdSaveRaw(filename))
		{
			if(!cv::imwrite(filename.toStdString(), actBScan->getRawImage()))
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
	const OctData::BScan* actBScan = markerManger.getActBScan();
	if(actBScan && existsRaw(actBScan))
	{
		QString filename = QFileDialog::getSaveFileName(this, tr("Save raw data as matrix"), "", "CV (*.xml *.jml)");
		if(!filename.isEmpty())
		{
			cv::FileStorage fs(filename.toStdString(), cv::FileStorage::WRITE);
			fs << "BScan" << actBScan->getRawImage();
		}
	}
}


void BScanMarkerWidget::saveRawBin()
{
	const OctData::BScan* actBScan = markerManger.getActBScan();
	if(actBScan && existsRaw(actBScan))
	{
		QString filename = QFileDialog::getSaveFileName(this, tr("Save raw data as bin"), "", "Binary (*.bin)");
		if(!filename.isEmpty())
		{
			CppFW::CVMatTreeStructBin::writeBin(filename.toStdString(), actBScan->getRawImage());
		}
	}
}

void BScanMarkerWidget::saveImageBin()
{
	const OctData::BScan* actBScan = markerManger.getActBScan();
	if(actBScan)
	{
		QString filename = QFileDialog::getSaveFileName(this, tr("Save image data as bin"), "", "Binary (*.bin)");
		if(!filename.isEmpty())
		{
			CppFW::CVMatTreeStructBin::writeBin(filename.toStdString(), actBScan->getImage());
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


void BScanMarkerWidget::updateGraphicsViewSize()
{
	gv->setGeometry(0, 0, scaledImageWidth(), scaledImageHeight());
}


void BScanMarkerWidget::showImage(const cv::Mat& image)
{
	CVImageWidget::showImage(image);

	triggerAutoImageFit();

	GraphicsView* gvConvert = dynamic_cast<GraphicsView*>(gv);
	if(gvConvert)
		gvConvert->setImageSize(image.cols, image.rows);
	updateGraphicsViewSize();
}


void BScanMarkerWidget::setPaintMarker(const PaintMarker* pm)
{
	if(paintMarker)
		disconnect(paintMarker, &PaintMarker::viewChanged, this, &BScanMarkerWidget::viewOptionsChangedSlot);

	paintMarker = pm;

	if(paintMarker)
		connect(paintMarker, &PaintMarker::viewChanged, this, &BScanMarkerWidget::viewOptionsChangedSlot);
}

void BScanMarkerWidget::triggerAutoImageFit()
{
	if(ProgramOptions::bscanAutoFitImage())
		setZoomInternal(getFactorFitImage2Parent());
}

void BScanMarkerWidget::setZoom(double factor)
{
	ProgramOptions::bscanAutoFitImage.setValue(false);
	CVImageWidget::setZoom(factor);
}




void BScanMarkerWidget::saveLatexImage()
{
	QString filename = QFileDialog::getSaveFileName(this, tr("Save image as LaTeX"), "", "LaTeX (*.tex)");
	if(!filename.isEmpty())
	{
		QFileInfo file(filename);
		QString basename = file.baseName();

		QString imageFilename        = basename + "_base.jpg";
		QString imageOverlayFilename = basename + "_overlay.png";

		QImage imageTmp;
		cvImage2qtImage(outputImage, imageTmp);
		imageTmp.save(file.path() + '/' + imageFilename);

		QImage overlay(imageTmp.size(), QImage::Format_ARGB32_Premultiplied);
		overlay.fill(qRgba(0, 0, 0, 0));
		QPainter segPainter(&overlay);
		paintSegmentations(segPainter, ScaleFactor());
		segPainter.end();

		BScanMarkerWidget fakeWidget;
		fakeWidget.resize(imageTmp.size());
		QPainter p(&overlay);
		if(paintMarker)
			paintMarker->paintMarker(p, &fakeWidget, imageTmp.rect());
		overlay.save(file.path() + '/' + imageOverlayFilename);



		std::ofstream stream(filename.toStdString());
		if(!stream.good())
			return;

		stream << "\\documentclass{standalone}\n\\usepackage{tikz}\n\n";
		stream << "\n\\begin{document}";
		stream << "\n\n\n\t\\begin{tikzpicture}";
		stream << "\n\n\t\t\\node[inner sep=0,scale=1] at (0,0) {\\includegraphics{" << imageFilename.toStdString() << "}};\n";
		stream << "\n\t\t\\node[inner sep=0, scale=1] at (0,0) {\\includegraphics{" << imageOverlayFilename.toStdString() << "}};\n";
		stream << "\n\t\\end{tikzpicture}";
		stream << "\n\\end{document}\n";
	}
}

