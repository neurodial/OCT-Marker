#include "stupidsplinewindow.h"

#include <QMenu>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QtGui>
#include <QPushButton>
#include <QToolButton>
#include <QProgressDialog>
#include <QSlider>
#include<QLabel>

#include <octdata/datastruct/series.h>

#include <widgets/slowithlegendwidget.h>
#include <widgets/sloimagewidget.h>
#include <widgets/bscanmarkerwidget.h>
#include <widgets/dwmarkerwidgets.h>
#include <widgets/scrollareapan.h>
#include <widgets/bscanchooserspinbox.h>
#include <widgets/octinformationwidgetaction.h>

#include <data_structure/programoptions.h>

#include <manager/octmarkermanager.h>
#include <manager/octdatamanager.h>
#include <manager/octmarkerio.h>
#include <manager/paintmarker.h>

#include <markermodules/bscanmarkerbase.h>
#include <markermodules/bscanlayersegmentation/bscanlayersegmentation.h>

#include <model/octfilesmodel.h>
#include <model/octdatamodel.h>

#include <boost/exception/exception.hpp>
#include <boost/exception/diagnostic_information.hpp>


StupidSplineWindow::StupidSplineWindow()
: QMainWindow()
, dwSloImage         (new QDockWidget(this))
, bscanMarkerWidget  (new BScanMarkerWidget)
, markerActions      (bscanMarkerWidget)
{
	setProgramOptions();
	setMinimumWidth(1000);
// 	setMinimumHeight(600);
	connect(ProgramOptions::getResetAction(), &QAction::triggered, this, &StupidSplineWindow::setProgramOptions);

	pmm = new PaintMarker();
	bscanMarkerWidget->setPaintMarker(pmm);


	bscanMarkerWidgetScrollArea = new ScrollAreaPan(this);
	bscanMarkerWidgetScrollArea->setWidget(bscanMarkerWidget);
	connect(bscanMarkerWidget, &CVImageWidget::needScrollTo, bscanMarkerWidgetScrollArea, &ScrollAreaPan::scrollTo);


	OctDataManager& octDataManager = OctDataManager::getInstance();
	connect(&octDataManager, &OctDataManager::loadFileSignal  , this, &StupidSplineWindow::loadFileStatusSlot);
	connect(&octDataManager, &OctDataManager::loadFileProgress, this, &StupidSplineWindow::loadFileProgress  );
	connect(&octDataManager, static_cast<void(OctDataManager::*)()>(&OctDataManager::octFileChanged), this, &StupidSplineWindow::updateWindowTitle );


	OctMarkerManager& marker = OctMarkerManager::getInstance();
	marker.setBscanMarkerTextID(QString("LayerSegmentation"));
	setIconsInMarkerWidget();



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


	QSettings& settings = ProgramOptions::getSettings();
	restoreGeometry(settings.value("stupidMainWindowGeometry").toByteArray());

	// General Config
	setWindowIcon(QIcon(":/icons/typicons/oct_marker_logo.svg"));
	updateWindowTitle();
}

StupidSplineWindow::~StupidSplineWindow()
{
	delete pmm;
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
	ProgramOptions::writeAllOptions();

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
			progressDialog = new QProgressDialog(tr("Opening file ..."), tr("Abort"), 0, 100, this);

// 		progressDialog->setCancelButtonText(nullptr);
		progressDialog->setWindowModality(Qt::WindowModal);
		progressDialog->setValue(0);
		progressDialog->setVisible(true);
		progressDialog->setAutoClose(false);

		OctDataManager& octDataManager = OctDataManager::getInstance();
		connect(progressDialog, &QProgressDialog::canceled, &octDataManager, &OctDataManager::abortLoadingOctFile);
	}
	else
	{
		if(progressDialog)
		{
			progressDialog->setVisible(false);

			progressDialog->deleteLater();
			progressDialog = nullptr;
		}
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

QAction* StupidSplineWindow::getDoubleSliderAction(OptionDouble& option, const QString& label)
{
	QWidgetAction* sloOverlayAlphaSliderAction = new QWidgetAction(this);

	QSlider* sloOverlayAlphaSlider = option.createSlider(Qt::Horizontal, this);
	QWidget* widget = new QWidget(this);
	QHBoxLayout* layout = new QHBoxLayout(widget);
	layout->addWidget(new QLabel(label, widget));
	layout->addWidget(sloOverlayAlphaSlider);
	widget->setLayout(layout);

	sloOverlayAlphaSliderAction->setDefaultWidget(widget);
	sloOverlayAlphaSliderAction->setIcon(QIcon(":/icons/tango/actions/document-save.svgz"));

	return sloOverlayAlphaSliderAction;
}


QDockWidget* StupidSplineWindow::createStupidControls()
{
	QHBoxLayout* layoutStupidControls = new QHBoxLayout;
	QFont textFont = QFont("Times", 20, QFont::Bold);


	// ----------------------
	// Oct informations
	// ----------------------
	OctInformationWidgetAction* octInformationAction = new OctInformationWidgetAction(this);
	QToolButton* buttonOctInformation = new QToolButton(this);
	buttonOctInformation->setFont(textFont);
	buttonOctInformation->setIconSize(buttonSize);
	buttonOctInformation->setDefaultAction(octInformationAction);
	buttonOctInformation->addAction(octInformationAction);
	buttonOctInformation->setPopupMode(QToolButton::InstantPopup);
	layoutStupidControls->addWidget(buttonOctInformation);

	addLayoutVLine(layoutStupidControls);


	BScanChooserSpinBox* bscanChooser = new BScanChooserSpinBox(this);
	bscanChooser->setFont(textFont);
	layoutStupidControls->addWidget(bscanChooser);

	// ----------------------
	addLayoutVLine(layoutStupidControls);


	layoutStupidControls->addWidget(genToolButton(markerActions.getZoomInAction ()));
	layoutStupidControls->addWidget(genToolButton(markerActions.getZoomOutAction()));
	layoutStupidControls->addWidget(genToolButton(ProgramOptions::bscanAutoFitImage.getAction()));


	// ----------------------
	addLayoutVLine(layoutStupidControls);
	QActionGroup* bscanAspectRatio = markerActions.getBscanAspectRatioActions();
	for(QAction* action : bscanAspectRatio->actions())
		layoutStupidControls->addWidget(genToolButton(action));


	// ----------------------
	addLayoutVLine(layoutStupidControls);


	layoutStupidControls->addWidget(genToolButton(markerActions.getUndoAction()));
	layoutStupidControls->addWidget(genToolButton(markerActions.getRedoAction()));

	addLayoutVLine(layoutStupidControls);
	layoutStupidControls->addWidget(genToolButton(ProgramOptions::sloClipScanArea.getAction()));

	// ----------------------
	layoutStupidControls->addStretch();
	// ----------------------



	QToolButton* buttonSaveAndClose = new QToolButton(this);
	buttonSaveAndClose->setText(tr("Save and Close"));
	buttonSaveAndClose->setToolTip(tr("Save and Close"));
	buttonSaveAndClose->setIcon(QIcon::fromTheme("document-save", QIcon(":/icons/tango/actions/document-save.svgz")));
	buttonSaveAndClose->setFont(textFont);
	buttonSaveAndClose->setIconSize(buttonSize);
	connect(buttonSaveAndClose, &QAbstractButton::clicked, this, &StupidSplineWindow::saveAndClose);
	layoutStupidControls->addWidget(buttonSaveAndClose);

	QToolButton* buttonClose = new QToolButton(this);
	buttonClose->setIcon(QIcon::fromTheme("application-exit", QIcon(":/icons/tango/actions/system-log-out.svgz")));
	buttonClose->setText(tr("Quit"));
	buttonClose->setToolTip(tr("Quit"));
	buttonClose->setFont(textFont);
	buttonClose->setIconSize(buttonSize);
	connect(buttonClose, &QAbstractButton::clicked, this, &StupidSplineWindow::close);
	layoutStupidControls->addWidget(buttonClose);

	// ----------------------
	addLayoutVLine(layoutStupidControls);

	QMenu* settingsMenu = new QMenu(this);
	settingsMenu->addSection(tr("View options"));

	settingsMenu->addAction(ProgramOptions::layerSegActiveLineColor.getColorDialogAction());
	settingsMenu->addAction(ProgramOptions::layerSegPassivLineColor.getColorDialogAction());

	settingsMenu->addAction(ProgramOptions::layerSegActiveLineSize .getInputDialogAction());
	settingsMenu->addAction(ProgramOptions::layerSegPassivLineSize .getInputDialogAction());
	settingsMenu->addAction(ProgramOptions::layerSegSplinePointSize.getInputDialogAction());

	settingsMenu->addAction(getDoubleSliderAction(ProgramOptions::sloOverlayAlpha, tr("SLO α value")));

	settingsMenu->addSection(tr("Spline interpolation options"));

	settingsMenu->addAction(ProgramOptions::layerSegFindPointMaxPoints  .getInputDialogAction());
	settingsMenu->addAction(ProgramOptions::layerSegFindPointMaxAbsError.getInputDialogAction());
	settingsMenu->addAction(ProgramOptions::layerSegFindPointRemoveTol  .getInputDialogAction());

	settingsMenu->addSection(tr("Performance options"));
	settingsMenu->addAction(ProgramOptions::layerSegSloMapsAutoUpdate.getAction());
	settingsMenu->addAction(ProgramOptions::layerSegHighlightSegLine .getAction());

	settingsMenu->addSection(tr("General"));
	settingsMenu->addAction(ProgramOptions::getResetAction());


	QToolButton* buttonSettings = new QToolButton(this);
	buttonSettings->setIcon(QIcon::fromTheme("preferences-system",  QIcon(":/icons/tango/categories/preferences-system.svgz")));
	buttonSettings->setText(tr("Settings"));
	buttonSettings->setFont(textFont);
	buttonSettings->setIconSize(buttonSize);
	buttonSettings->setMenu(settingsMenu);
	buttonSettings->setPopupMode(QToolButton::InstantPopup);
	layoutStupidControls->addWidget(buttonSettings);

	markerActions.getAboutDialogAction()->setIcon(QIcon::fromTheme("dialog-information",  QIcon(":/icons/tango/apps/help-browser.svgz")));
	layoutStupidControls->addWidget(genToolButton(markerActions.getAboutDialogAction()));




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

void StupidSplineWindow::setProgramOptions()
{
	ProgramOptions::bscansShowSegmentationslines.setValue(false);
	ProgramOptions::bscanAspectRatioType.setValue(2); // best fit
	ProgramOptions::bscanAutoFitImage.setValue(true);
}

