#include "stupidsplinewindow.h"

#include <QMenu>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QtGui>

#include <QProgressDialog>
#include <QLabel>

#include <widgets/sloimagewidget.h>
#include <widgets/bscanmarkerwidget.h>
#include <widgets/dwmarkerwidgets.h>
#include <widgets/scrollareapan.h>

#include <data_structure/programoptions.h>

#include <manager/octmarkermanager.h>
#include <manager/octdatamanager.h>
#include <manager/octmarkerio.h>
#include <markermodules/bscanmarkerbase.h>

#include <model/octfilesmodel.h>
#include <model/octdatamodel.h>

#include <octdata/datastruct/sloimage.h>
#include <octdata/datastruct/bscan.h>
#include <octdata/datastruct/series.h>

#include <octdata/octfileread.h>
#include <octdata/filereadoptions.h>

#include <boost/exception/exception.hpp>
#include <boost/exception/diagnostic_information.hpp>

#include <globaldefinitions.h>

#include <QPushButton>
#include <widgets/dwimagecoloradjustments.h>
#include <QToolButton>

#include<windows/infodialogs.h>
#include <markermodules/bscanlayersegmentation/bscanlayersegmentation.h>


namespace
{
	class SimpleWgSloImage : public QMainWindow
	{
		SLOImageWidget* imageWidget;
	public:
		explicit SimpleWgSloImage(QWidget* parent = nullptr) : QMainWindow(parent), imageWidget(new SLOImageWidget(this))
		{
			setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
			imageWidget->setImageSize(size());
			setCentralWidget(imageWidget);
		}
		SLOImageWidget* getImageWidget() { return imageWidget; }

	protected:
		virtual void resizeEvent(QResizeEvent* event) override
		{
			imageWidget->setImageSize(event->size());
			QWidget::resizeEvent(event);
		}
	};

}


StupidSplineWindow::StupidSplineWindow()
: QMainWindow()
, dwSloImage         (new QDockWidget(this))
, bscanMarkerWidget  (new BScanMarkerWidget)
{
	ProgramOptions::bscansShowSegmentationslines.setValue(false);
	setMinimumWidth(1000);
// 	setMinimumHeight(600);

	bscanMarkerWidgetScrollArea = new ScrollAreaPan(this);
	bscanMarkerWidgetScrollArea->setWidget(bscanMarkerWidget);
	connect(bscanMarkerWidget, &CVImageWidget::needScrollTo, bscanMarkerWidgetScrollArea, &ScrollAreaPan::scrollTo);


	OctDataManager& octDataManager = OctDataManager::getInstance();
	connect(&octDataManager, &OctDataManager::loadFileSignal  , this, &StupidSplineWindow::loadFileStatusSlot);
	connect(&octDataManager, &OctDataManager::loadFileProgress, this, &StupidSplineWindow::loadFileProgress  );

	updateWindowTitle();


	QSettings& settings = ProgramOptions::getSettings();
	restoreGeometry(settings.value("stupidMainWindowGeometry").toByteArray());

	
	// General Objects
	setCentralWidget(bscanMarkerWidgetScrollArea);


	QHBoxLayout* layoutZoomControl = new QHBoxLayout;

	QSize buttonSize(50, 50);

	QToolButton* infoButton = new QToolButton(this);
	infoButton->setIcon(QIcon(":/icons/typicons/info-large-outline.svg"));
	infoButton->setIconSize(buttonSize);
	infoButton->setToolTip(tr("About"));
	connect(infoButton, &QToolButton::clicked, this, &StupidSplineWindow::showAboutDialog);
	layoutZoomControl->addWidget(infoButton);


	zoomInAction = new QAction(this);
	zoomInAction->setText(tr("Zoom +"));
	zoomInAction->setIcon(QIcon(":/icons/typicons/zoom-in-outline.svg"));
	connect(zoomInAction, &QAction::triggered, bscanMarkerWidget, &CVImageWidget::zoom_in);
	QToolButton* buttonZoomIn = new QToolButton(this);
	buttonZoomIn->setDefaultAction(zoomInAction);
	buttonZoomIn->setIconSize(buttonSize);
	layoutZoomControl->addWidget(buttonZoomIn);

	zoomOutAction = new QAction(this);
	zoomOutAction->setText(tr("Zoom -"));
	zoomOutAction->setIcon(QIcon(":/icons/typicons/zoom-out-outline.svg"));
	connect(zoomOutAction, &QAction::triggered, bscanMarkerWidget, &CVImageWidget::zoom_out);
	QToolButton* buttonZoomOut = new QToolButton(this);
	buttonZoomOut->setDefaultAction(zoomOutAction);
	buttonZoomOut->setIconSize(buttonSize);
	layoutZoomControl->addWidget(buttonZoomOut);

	zoomFitAction = new QAction(this);
	zoomFitAction->setText(tr("fit image"));
	zoomFitAction->setIcon(QIcon(":/icons/typicons/arrow-maximise-outline.svg"));
	connect(zoomFitAction, &QAction::triggered, this, &StupidSplineWindow::fitBScanImage2Widget);
	QToolButton* buttonZoomFit = new QToolButton(this);
	buttonZoomFit->setDefaultAction(zoomFitAction);
	buttonZoomFit->setIconSize(buttonSize);
	layoutZoomControl->addWidget(buttonZoomFit);


	QWidget* widgetZoomControl = new QWidget();
	widgetZoomControl->setLayout(layoutZoomControl);

	QDockWidget* dwZoomControl = new QDockWidget(this);
	dwZoomControl->setWindowTitle("Buttons");
	dwZoomControl->setWidget(widgetZoomControl);
	dwZoomControl->setFeatures(0);
	dwZoomControl->setObjectName("dwZoomControl");
	dwZoomControl->setTitleBarWidget(new QWidget());
	addDockWidget(Qt::LeftDockWidgetArea, dwZoomControl);



	SimpleWgSloImage* sloImageWidget = new SimpleWgSloImage(this);
	sloImageWidget->setMinimumWidth(250);
	sloImageWidget->setMinimumHeight(250);
	dwSloImage->setWindowTitle("SLO");
	dwSloImage->setWidget(sloImageWidget);
	dwSloImage->setFeatures(0);
	dwSloImage->setObjectName("StupidDWSloImage");
	dwSloImage->setTitleBarWidget(new QWidget());
	addDockWidget(Qt::LeftDockWidgetArea, dwSloImage);
	connect(bscanMarkerWidget, &BScanMarkerWidget::mousePosOnBScan, sloImageWidget->getImageWidget(), &SLOImageWidget::showPosOnBScan);


	OctMarkerManager& marker = OctMarkerManager::getInstance();
	marker.setBscanMarkerTextID(QString("LayerSegmentation"));

	DWMarkerWidgets* dwmarkerwidgets = new DWMarkerWidgets(this);
	dwmarkerwidgets->setObjectName("DwMarkerWidgets");
	dwmarkerwidgets->setFeatures(0);
	dwmarkerwidgets->setTitleBarWidget(new QWidget());
	addDockWidget(Qt::LeftDockWidgetArea, dwmarkerwidgets);



	QDockWidget* dwSaveAndClose = new QDockWidget(this);
	QWidget* widgetSaveAndClose = new QWidget(dwSaveAndClose);
	dwSaveAndClose->setFeatures(0);
	dwSaveAndClose->setWindowTitle(tr("Quit"));
	dwSaveAndClose->setObjectName("dwSaveAndClose");
	QVBoxLayout* dcSaveAndCloseLayout = new QVBoxLayout(widgetSaveAndClose);

	QPushButton* buttonSaveAndClose = new QPushButton(this);
	buttonSaveAndClose->setText(tr("Save and Close"));
	buttonSaveAndClose->setFont(QFont("Times", 24, QFont::Bold));
	connect(buttonSaveAndClose, &QAbstractButton::clicked, this, &StupidSplineWindow::saveAndClose);
	dcSaveAndCloseLayout->addWidget(buttonSaveAndClose);

	QPushButton* buttonClose = new QPushButton(this);
	buttonClose->setText(tr("Quit"));
	 buttonClose->setFont(QFont("Times", 24, QFont::Bold));
	connect(buttonClose, &QAbstractButton::clicked, this, &StupidSplineWindow::close);
	dcSaveAndCloseLayout->addWidget(buttonClose);

	widgetSaveAndClose->setLayout(dcSaveAndCloseLayout);
	dwSaveAndClose->setWidget(widgetSaveAndClose);
	dwSaveAndClose->setTitleBarWidget(new QWidget());
	addDockWidget(Qt::LeftDockWidgetArea, dwSaveAndClose);



	// General Config
	setWindowIcon(QIcon(":/icons/typicons/oct_marker_logo.svg"));

	connect(bscanMarkerWidget, &CVImageWidget::zoomChanged, this, &StupidSplineWindow::zoomChanged);
	connect(&OctDataManager::getInstance(), &OctDataManager::seriesChanged, this, &StupidSplineWindow::fitBScanImage2Widget);

	setIconsInMarkerWidget();
}

StupidSplineWindow::~StupidSplineWindow()
{
}



void StupidSplineWindow::zoomChanged(double zoom)
{
	if(zoomInAction ) zoomInAction ->setEnabled(zoom < 8);
	if(zoomOutAction) zoomOutAction->setEnabled(zoom > 0.5);
}


BScanLayerSegmentation* StupidSplineWindow::getLayerSegmentationModul()
{
	OctMarkerManager& markerManager = OctMarkerManager::getInstance();
	BscanMarkerBase* markerModul = markerManager.getActBscanMarker();

	if(!markerModul)
		return nullptr;

	return dynamic_cast<BScanLayerSegmentation*>(markerModul);
}


bool StupidSplineWindow::saveLayerSegmentation()
{
	BScanLayerSegmentation* layerSegmentationModul = getLayerSegmentationModul();
	if(!layerSegmentationModul)
		return false;

	QString filename = OctDataManager::getInstance().getLoadedFilename() + ".segmentation.bin";
	bool saveResult = layerSegmentationModul->saveSegmentation2Bin(filename.toStdString());
	saved = saveResult;
	return saveResult;
}

bool StupidSplineWindow::copyLayerSegmentationFromOCTData()
{
	BScanLayerSegmentation* layerSegmentationModul = getLayerSegmentationModul();
	if(!layerSegmentationModul)
		return false;

	layerSegmentationModul->copyAllSegLinesFromOctData();
	return true;
}


bool StupidSplineWindow::setIconsInMarkerWidget()
{
	BScanLayerSegmentation* layerSegmentationModul = getLayerSegmentationModul();
	if(!layerSegmentationModul)
		return false;

	layerSegmentationModul->setIconsToSimple(40);
	return true;
}


void StupidSplineWindow::closeEvent(QCloseEvent* e)
{
	if(!saved)
	{
		int ret = QMessageBox::warning(this, tr("Data not saved"), tr("Data not saved!<br />Quit program?"), QMessageBox::Yes | QMessageBox::No);
		if(ret == QMessageBox::No)
			return e->ignore();
	}


	// save programoptions
// 	ProgramOptions::loadOctdataAtStart.setValue(OctDataManager::getInstance().getLoadedFilename());
// 	ProgramOptions::writeAllOptions();

	QSettings& settings = ProgramOptions::getSettings();
	

	settings.setValue("stupidMainWindowGeometry", saveGeometry());
	settings.setValue("stupidMainWindowState"   , saveState()   );

	QWidget::closeEvent(e);
}




void StupidSplineWindow::loadFileStatusSlot(bool loading)
{
	if(loading)
	{
		if(!progressDialog)
			progressDialog = new QProgressDialog("Opening file ...", "Abort", 0, 100, this);

		progressDialog->setCancelButtonText(nullptr);
		progressDialog->setWindowModality(Qt::WindowModal);
		progressDialog->setValue(0);
		progressDialog->setVisible(true);
	}
	else
	{
		if(progressDialog)
		{
			progressDialog->setVisible(false);

			progressDialog->deleteLater();
			progressDialog = nullptr;
		}
		updateWindowTitle();
		copyLayerSegmentationFromOCTData();
	}
}


void StupidSplineWindow::loadFileProgress(double frac)
{
	if(progressDialog)
		progressDialog->setValue(static_cast<int>(frac*100));
}



bool StupidSplineWindow::loadFile(const QString& filename)
{
	return OctFilesModel::getInstance().loadFile(filename);
}

void StupidSplineWindow::showAboutDialog()
{
	InfoDialogs::showAboutDialog(this);
}

void StupidSplineWindow::fitBScanImage2Widget()
{
	bscanMarkerWidget->fitImage(bscanMarkerWidgetScrollArea->width () - 2
	                          , bscanMarkerWidgetScrollArea->height() - 2);
}

void StupidSplineWindow::updateWindowTitle()
{
	const QString& filename = OctDataManager::getInstance().getLoadedFilename();

	if(filename.isEmpty())
		setWindowTitle(tr("OCT-Marker - simple spline gui"));
	else
		setWindowTitle(tr("OCT-Marker - simple spline gui") + " - " + filename);
}

void StupidSplineWindow::saveAndClose()
{
	if(!saveLayerSegmentation())
		QMessageBox::critical(this, tr("Error on save"), tr("Internal error in saveLayerSegmentation()"));
	else
		close();
}
