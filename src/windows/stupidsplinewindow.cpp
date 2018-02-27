#include "stupidsplinewindow.h"

#include <QMenu>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QtGui>

#include <QProgressDialog>
#include <QLabel>

#include <widgets/slowithlegendwidget.h>
#include <widgets/sloimagewidget.h>
#include <widgets/bscanmarkerwidget.h>
#include <widgets/dwmarkerwidgets.h>
#include <widgets/scrollareapan.h>

#include <data_structure/programoptions.h>

#include <manager/octmarkermanager.h>
#include <manager/octdatamanager.h>
#include <manager/octmarkerio.h>
#include <manager/paintmarker.h>
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
#include <QSpinBox>

#include<QDesktopWidget>


StupidSplineWindow::StupidSplineWindow()
: QMainWindow()
, dwSloImage         (new QDockWidget(this))
, bscanMarkerWidget  (new BScanMarkerWidget)
, markerActions      (bscanMarkerWidget)
{
	ProgramOptions::bscansShowSegmentationslines.setValue(false);
	setMinimumWidth(1000);
// 	setMinimumHeight(600);

	pmm = new PaintMarker();
	bscanMarkerWidget->setPaintMarker(pmm);


	bscanMarkerWidgetScrollArea = new ScrollAreaPan(this);
	bscanMarkerWidgetScrollArea->setWidget(bscanMarkerWidget);
	connect(bscanMarkerWidget, &CVImageWidget::needScrollTo, bscanMarkerWidgetScrollArea, &ScrollAreaPan::scrollTo);


	OctDataManager& octDataManager = OctDataManager::getInstance();
	connect(&octDataManager, &OctDataManager::loadFileSignal  , this, &StupidSplineWindow::loadFileStatusSlot);
	connect(&octDataManager, &OctDataManager::loadFileProgress, this, &StupidSplineWindow::loadFileProgress  );

	updateWindowTitle();

	OctMarkerManager& marker = OctMarkerManager::getInstance();
	marker.setBscanMarkerTextID(QString("LayerSegmentation"));
	setIconsInMarkerWidget();
	connect(&marker, &OctMarkerManager::undoRedoStateChange, this, &StupidSplineWindow::updateRedoUndoButtons);

	QSettings& settings = ProgramOptions::getSettings();
	restoreGeometry(settings.value("stupidMainWindowGeometry").toByteArray());

	
	// General Objects
	setCentralWidget(bscanMarkerWidgetScrollArea);


	addDockWidget(Qt::TopDockWidgetArea, createStupidControls());


	SloWithLegendWidget* sloImageWidget = new SloWithLegendWidget(this);
	sloImageWidget->setMinimumWidth(250);
	sloImageWidget->setMinimumHeight(250);
	dwSloImage->setWindowTitle("SLO");
	dwSloImage->setWidget(sloImageWidget);
	dwSloImage->setFeatures(QDockWidget::NoDockWidgetFeatures);
	dwSloImage->setObjectName("StupidDWSloImage");
	dwSloImage->setTitleBarWidget(new QWidget());
	addDockWidget(Qt::LeftDockWidgetArea, dwSloImage);
	connect(bscanMarkerWidget, &BScanMarkerWidget::mousePosOnBScan, sloImageWidget->getImageWidget(), &SLOImageWidget::showPosOnBScan);



	DWMarkerWidgets* dwmarkerwidgets = new DWMarkerWidgets(this);
	dwmarkerwidgets->setObjectName("DwMarkerWidgets");
	dwmarkerwidgets->setFeatures(QDockWidget::NoDockWidgetFeatures);
	dwmarkerwidgets->setTitleBarWidget(new QWidget());
// 	dwmarkerwidgets->setFixedHeight(dwmarkerwidgets->minimumSizeHint().height());
	dwmarkerwidgets->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
	addDockWidget(Qt::LeftDockWidgetArea, dwmarkerwidgets);


	// General Config
	setWindowIcon(QIcon(":/icons/typicons/oct_marker_logo.svg"));

	connect(bscanMarkerWidget, &CVImageWidget::zoomChanged, this, &StupidSplineWindow::zoomChanged);
	connect(&OctDataManager::getInstance(), &OctDataManager::seriesChanged, this, &StupidSplineWindow::fitBScanImage2Widget);

}

StupidSplineWindow::~StupidSplineWindow()
{
	delete pmm;
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

	layerSegmentationModul->setIconsToSimple(32);
	return true;
}

bool StupidSplineWindow::hasDataChange()
{
	OctMarkerManager& markerManager = OctMarkerManager::getInstance();
	BscanMarkerBase* markerModul = markerManager.getActBscanMarker();

	assert(markerModul != nullptr);
	if(!markerModul)
		return true; // TODO: release mode: internal program error

	return markerModul->hasChangedSinceLastSave();
}



void StupidSplineWindow::closeEvent(QCloseEvent* e)
{
	if(!saved && hasDataChange())
	{
		int ret = QMessageBox::warning(this, tr("Data not saved"), tr("You have unsaved changes, what will you do?"), QMessageBox::Discard | QMessageBox::Cancel | QMessageBox::Save);
		if(ret == QMessageBox::Cancel)
			return e->ignore();
		if(ret == QMessageBox::Save)
			if(!saveLayerSegmentation())
			{
				QMessageBox::critical(this, tr("Data cant saved"), tr("The data cant be saved, abort closing"));
				return e->ignore();
			}
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

namespace
{
	void addLayoutVLine(QLayout* layout)
	{
		layout->setSpacing(10);

		QFrame* line = new QFrame();
		// line->setGeometry(QRect(10,30));
		line->setFrameShape(QFrame::VLine); // Replace by VLine for vertical line
		line->setFrameShadow(QFrame::Sunken);
		layout->addWidget(line);

		layout->setSpacing(10);
	}
}

QToolButton* StupidSplineWindow::genToolButton(QAction* action)
{
	QToolButton* button = new QToolButton(this);
	button->setDefaultAction(action);
	button->setIconSize(buttonSize);
	return button;
}

QDockWidget* StupidSplineWindow::createStupidControls()
{
	QHBoxLayout* layoutStupidControls = new QHBoxLayout;
	QFont textFont = QFont("Times", 20, QFont::Bold);

	OctMarkerManager& markerManager = OctMarkerManager::getInstance();
	BScanLayerSegmentation* layerSeg = getLayerSegmentationModul();

	bscanChooser = new QSpinBox(this);
	bscanChooser->setFont(textFont);
	connect(bscanChooser, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), &markerManager, &OctMarkerManager::chooseBScan);
// 	connect(bscanChooser, SIGNAL(valueChanged(int)), &markerManager, SLOT(chooseBScan(int)));
	connect(&markerManager, &OctMarkerManager::bscanChanged, bscanChooser, &QSpinBox::setValue);
	connect(&markerManager, &OctMarkerManager::newSeriesShowed, this, &StupidSplineWindow::updateBScanChooser);

	labelMaxBscan = new QLabel(this);
	labelMaxBscan->setFont(textFont);
	labelMaxBscan->setText("/0");

	layoutStupidControls->addWidget(bscanChooser);
	layoutStupidControls->addWidget(labelMaxBscan);

	// ----------------------
	addLayoutVLine(layoutStupidControls);


	layoutStupidControls->addWidget(genToolButton(markerActions.getZoomInAction ()));
	layoutStupidControls->addWidget(genToolButton(markerActions.getZoomOutAction()));


	zoomFitAction = new QAction(this);
	zoomFitAction->setText(tr("fit image"));
	zoomFitAction->setIcon(QIcon::fromTheme("zoom-fit-best",  QIcon(":/icons/tango/actions/view-fullscreen.svgz")));
	connect(zoomFitAction, &QAction::triggered, this, &StupidSplineWindow::fitBScanImage2Widget);
	QToolButton* buttonZoomFit = new QToolButton(this);
	buttonZoomFit->setDefaultAction(zoomFitAction);
	buttonZoomFit->setIconSize(buttonSize);
	layoutStupidControls->addWidget(buttonZoomFit);

	// ----------------------
	addLayoutVLine(layoutStupidControls);


	buttonUndo = new QToolButton(this);
	buttonUndo->setText(tr("undo"));
	buttonUndo->setToolTip(tr("undo"));
	buttonUndo->setIcon(QIcon::fromTheme("edit-undo", QIcon(":/icons/tango/actions/edit-undo.svgz")));
	buttonUndo->setFont(QFont("Times", 24, QFont::Bold));
	buttonUndo->setIconSize(buttonSize);
	buttonUndo->setEnabled(false);
	connect(buttonUndo, &QAbstractButton::clicked, layerSeg, &BscanMarkerBase::callUndoStep);
	layoutStupidControls->addWidget(buttonUndo);


	buttonRedo = new QToolButton(this);
	buttonRedo->setText(tr("redo"));
	buttonRedo->setToolTip(tr("redo"));
	buttonRedo->setIcon(QIcon::fromTheme("edit-redo", QIcon(":/icons/tango/actions/edit-redo.svgz")));
	buttonRedo->setFont(QFont("Times", 24, QFont::Bold));
	buttonRedo->setIconSize(buttonSize);
	buttonRedo->setEnabled(false);
	connect(buttonRedo, &QAbstractButton::clicked, layerSeg, &BscanMarkerBase::callRedoStep);
	layoutStupidControls->addWidget(buttonRedo);


	// ----------------------
	layoutStupidControls->addStretch();
	// ----------------------



	QToolButton* buttonSaveAndClose = new QToolButton(this);
	buttonSaveAndClose->setText(tr("Save and Close"));
	buttonSaveAndClose->setToolTip(tr("Save and Close"));
	buttonSaveAndClose->setIcon(QIcon::fromTheme("document-save", QIcon(":/icons/tango/actions/document-save.svgz")));
	buttonSaveAndClose->setFont(QFont("Times", 24, QFont::Bold));
	buttonSaveAndClose->setIconSize(buttonSize);
	connect(buttonSaveAndClose, &QAbstractButton::clicked, this, &StupidSplineWindow::saveAndClose);
	layoutStupidControls->addWidget(buttonSaveAndClose);

	QToolButton* buttonClose = new QToolButton(this);
	buttonClose->setIcon(QIcon::fromTheme("application-exit", QIcon(":/icons/tango/actions/system-log-out.svgz")));
	buttonClose->setText(tr("Quit"));
	buttonClose->setToolTip(tr("Quit"));
	buttonClose->setFont(QFont("Times", 24, QFont::Bold));
	buttonClose->setIconSize(buttonSize);
	connect(buttonClose, &QAbstractButton::clicked, this, &StupidSplineWindow::close);
	layoutStupidControls->addWidget(buttonClose);

	// ----------------------
	addLayoutVLine(layoutStupidControls);

	QToolButton* infoButton = new QToolButton(this);
	infoButton->setIcon(QIcon::fromTheme("dialog-information",  QIcon(":/icons/tango/apps/help-browser.svgz")));
	infoButton->setIconSize(buttonSize);
	infoButton->setToolTip(tr("About"));
	connect(infoButton, &QToolButton::clicked, this, &StupidSplineWindow::showAboutDialog);
	layoutStupidControls->addWidget(infoButton);




	QWidget* widgetZoomControl = new QWidget();
	widgetZoomControl->setLayout(layoutStupidControls);

	QDockWidget* dwZoomControl = new QDockWidget(this);
	dwZoomControl->setWindowTitle("Buttons");
	dwZoomControl->setWidget(widgetZoomControl);
	dwZoomControl->setFeatures(0);
	dwZoomControl->setObjectName("dwZoomControl");
	dwZoomControl->setTitleBarWidget(new QWidget());
	dwZoomControl->setFixedHeight(dwZoomControl->minimumSizeHint().height());

	return dwZoomControl;
}


void StupidSplineWindow::updateBScanChooser()
{
	const OctData::Series* series = OctDataManager::getInstance().getSeries();

	std::size_t maxBscan = 0;
	if(series)
	{
		maxBscan = series->bscanCount();
		if(maxBscan > 0)
			--maxBscan;
	}

	bscanChooser->setMaximum(static_cast<int>(maxBscan));
	bscanChooser->setValue(OctMarkerManager::getInstance().getActBScanNum());

	labelMaxBscan->setText(QString("/%1").arg(maxBscan));
}

void StupidSplineWindow::updateRedoUndoButtons()
{
	OctMarkerManager& markerManager = OctMarkerManager::getInstance();

	if(buttonUndo)
		buttonUndo->setEnabled(markerManager.numUndoSteps() > 0);
	if(buttonRedo)
		buttonRedo->setEnabled(markerManager.numRedoSteps() > 0);
}
