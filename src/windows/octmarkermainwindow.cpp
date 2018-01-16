#include "octmarkermainwindow.h"

#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QToolBar>
#include <QFileDialog>
#include <QStringList>
#include <QSignalMapper>
#include <QtGui>

#include <QActionGroup>
#include <QSpinBox>
#include <QProgressBar>
#include <QLabel>
#include <QStatusBar>

#include <widgets/wgsloimage.h>
#include <widgets/bscanmarkerwidget.h>
#include <widgets/wgoctdatatree.h>
#include <widgets/dwoctinformations.h>
#include <widgets/dwmarkerwidgets.h>
#include <widgets/scrollareapan.h>
#include <widgets/mousecoordstatus.h>
#include <widgets/dwdebugoutput.h>
#include <widgets/sloimagewidget.h>

#include <data_structure/intervalmarker.h>
#include <data_structure/programoptions.h>

#include <manager/octmarkermanager.h>
#include <manager/octdatamanager.h>
#include <manager/octmarkerio.h>
#include <markermodules/bscanmarkerbase.h>

#include <model/octfilesmodel.h>
#include <model/octdatamodel.h>
#include <model/paintmarkermodel.h>

#include <octdata/datastruct/sloimage.h>
#include <octdata/datastruct/bscan.h>
#include <octdata/datastruct/series.h>

#include <octdata/octfileread.h>
#include <octdata/filereadoptions.h>

#include <boost/exception/exception.hpp>
#include <boost/exception/diagnostic_information.hpp>

#include <boost/algorithm/string/join.hpp>

#include <cpp_framework/cvmat/treestructbin.h>

#include <globaldefinitions.h>
#include <buildconstants.h>
#include <QWidgetAction>


#include <QPushButton>
#include <widgets/dwimagecoloradjustments.h>

#include<windows/infodialogs.h>
#include <helper/actionclasses.h>
#include <widgets/wgoctmarkers.h>
#include <manager/paintmarker.h>

DWDebugOutput* OCTMarkerMainWindow::dwDebugOutput = nullptr;



OCTMarkerMainWindow::OCTMarkerMainWindow(bool loadLastFile)
: QMainWindow()
, dwSloImage         (new QDockWidget(this))
, bscanMarkerWidget  (new BScanMarkerWidget)
{
	OctMarkerManager& markerManager = OctMarkerManager::getInstance();
	QSettings& settings = ProgramOptions::getSettings();
	restoreGeometry(settings.value("mainWindowGeometry").toByteArray());
	
	
	bscanMarkerWidgetScrollArea = new ScrollAreaPan(this);
	bscanMarkerWidgetScrollArea->setWidget(bscanMarkerWidget);
	connect(bscanMarkerWidget, &CVImageWidget::needScrollTo, bscanMarkerWidgetScrollArea, &ScrollAreaPan::scrollTo);


	// General Objects
	setCentralWidget(bscanMarkerWidgetScrollArea);

	setupMenu();
	setupStatusBar();
	createMarkerToolbar();
	// DockWidgets
	WgSloImage* wgSloImage = new WgSloImage(this);
	dwSloImage->setObjectName("DWSloImage");
	dwSloImage->setWidget(wgSloImage);
	dwSloImage->setWindowTitle(tr("SLO image"));
	addDockWidget(Qt::LeftDockWidgetArea, dwSloImage);

	connect(bscanMarkerWidget, &BScanMarkerWidget::mousePosOnBScan, wgSloImage->getImageWidget(), &SLOImageWidget::showPosOnBScan);

	DWImageColorAdjustments* dwImageColorAdjustments = new DWImageColorAdjustments(this);
	dwImageColorAdjustments->setObjectName("DWImageContrast");
	addDockWidget(Qt::LeftDockWidgetArea, dwImageColorAdjustments);
	bscanMarkerWidget->setImageFilter(dwImageColorAdjustments->getImageFilter());

	if(!dwDebugOutput)
		dwDebugOutput = new DWDebugOutput(this);
	dwDebugOutput->setObjectName("DWDebugOutput");
	addDockWidget(Qt::RightDockWidgetArea, dwDebugOutput);


	WGOctDataTree* tree = new WGOctDataTree();
	QDockWidget* treeDock = new QDockWidget(tr("Oct Data"), this);
	treeDock->setObjectName("DWOctDataTree");
	treeDock->setWidget(tree);
	addDockWidget(Qt::RightDockWidgetArea, treeDock);
	
	DwOctInformations* dwoctinformations = new DwOctInformations(this);
	dwoctinformations->setObjectName("DwOctInformations");
	addDockWidget(Qt::RightDockWidgetArea, dwoctinformations);

	DWMarkerWidgets* dwmarkerwidgets = new DWMarkerWidgets(this);
	dwmarkerwidgets->setObjectName("DwMarkerWidgets");
	addDockWidget(Qt::LeftDockWidgetArea, dwmarkerwidgets);



	pmm = new PaintMarker();
	QDockWidget* markersDock = new QDockWidget(tr("Oct markers"), this);
	WGOctMarkers* markerswidgets = new WGOctMarkers(&(pmm->getModel()));
	markersDock->setObjectName("DwMarkersWidgets");
	markersDock->setWidget(markerswidgets);
	addDockWidget(Qt::LeftDockWidgetArea, markersDock);

	bscanMarkerWidget->setPaintMarker(pmm);



	// General Config
	setWindowIcon(QIcon(":/icons/typicons/oct_marker_logo.svg"));
	// setActionToggel();
	setAcceptDrops(true);

	connect(&OctDataManager::getInstance(), &OctDataManager::seriesChanged   , this, &OCTMarkerMainWindow::newCscanLoaded);
	connect(bscanMarkerWidget, &CVImageWidget::zoomChanged, this, &OCTMarkerMainWindow::zoomChanged);

	
	for(BscanMarkerBase* marker : markerManager.getBscanMarker())
	{
		QToolBar* toolbar = marker->createToolbar(this);
		if(toolbar)
			addToolBar(toolbar);
	}

	restoreState(settings.value("mainWindowState").toByteArray());

	if(loadLastFile && !ProgramOptions::loadOctdataAtStart().isEmpty())
		loadFile(ProgramOptions::loadOctdataAtStart());


	qInstallMessageHandler(OCTMarkerMainWindow::messageOutput);
}

OCTMarkerMainWindow::~OCTMarkerMainWindow()
{
	delete pmm;
}

void OCTMarkerMainWindow::messageOutput(QtMsgType type, const QMessageLogContext& context, const QString& msg)
{
	if(dwDebugOutput)
		dwDebugOutput->printMessages(type, context, msg);
}


namespace
{
	void addMenuProgramOptionGroup(const QString& name, OptionInt& obj, QMenu* menu, SendInt& sendObj, QActionGroup* group, QObject* parent)
	{
		QAction* action = new QAction(parent);
		action->setText(name);
		action->setCheckable(true);
		action->setActionGroup(group);
		sendObj.connectOptions(obj, action);

		menu->addAction(action);
	}

}



void OCTMarkerMainWindow::setupMenu()
{
	OctMarkerManager& markerManager = OctMarkerManager::getInstance();

	// ----------
	// fileMenu
	// ----------
	QMenu* fileMenu = new QMenu(this);
	fileMenu->setTitle(tr("File"));

	QAction* actionLoadImage = new QAction(this);
	actionLoadImage->setText(tr("Load OCT scan"));
	actionLoadImage->setShortcut(Qt::CTRL | Qt::Key_O);
	actionLoadImage->setIcon(QIcon(":/icons/folder_image.png"));
	connect(actionLoadImage, &QAction::triggered, this, &OCTMarkerMainWindow::showLoadImageDialog);
	fileMenu->addAction(actionLoadImage);

	QAction* actionImportFolder = new QAction(this);
	actionImportFolder->setText(tr("Add OCT scans from folder"));
	actionImportFolder->setShortcut(Qt::CTRL | Qt::SHIFT | Qt::Key_O);
	actionImportFolder->setIcon(QIcon(":/icons/folder_add.png"));
	connect(actionImportFolder, &QAction::triggered, this, &OCTMarkerMainWindow::showImportFromFolderDialog);
	fileMenu->addAction(actionImportFolder);

	fileMenu->addSeparator();

	QAction* loadMarkersAction = new QAction(this);
	loadMarkersAction->setText(tr("load markers"));
	loadMarkersAction->setIcon(QIcon(":/icons/folder.png"));
	connect(loadMarkersAction, &QAction::triggered, this, &OCTMarkerMainWindow::showLoadMarkersDialog);
	fileMenu->addAction(loadMarkersAction);

/*
	QAction* addMarkersAction = new QAction(this);
	addMarkersAction->setText(tr("add markers"));
	addMarkersAction->setIcon(QIcon(":/icons/folder_add.png"));
	connect(addMarkersAction, SIGNAL(triggered()), SLOT(showAddMarkersDialog()));
	fileMenu->addAction(addMarkersAction);
*/

	QAction* saveMarkersAction = new QAction(this);
	saveMarkersAction->setText(tr("save markers"));
	saveMarkersAction->setIcon(QIcon(":/icons/disk.png"));
	connect(saveMarkersAction, &QAction::triggered, this, &OCTMarkerMainWindow::showSaveMarkersDialog);
	fileMenu->addAction(saveMarkersAction);

	QAction* saveMarkersDefaultAction = new QAction(this);
	saveMarkersDefaultAction->setText(tr("trigger auto save markers"));
	saveMarkersDefaultAction->setIcon(QIcon(":/icons/disk.png"));
	saveMarkersDefaultAction->setShortcut(Qt::CTRL | Qt::Key_S);
	connect(saveMarkersDefaultAction, &QAction::triggered, &(OctDataManager::getInstance()), &OctDataManager::triggerSaveMarkersDefault);
	fileMenu->addAction(saveMarkersDefaultAction);


	fileMenu->addSeparator();

	QAction* saveOctScanAction = new QAction(this);
	saveOctScanAction->setText(tr("save oct scan"));
	saveOctScanAction->setIcon(QIcon(":/icons/disk.png"));
	connect(saveOctScanAction, &QAction::triggered, this, &OCTMarkerMainWindow::showSaveOctScanDialog);
	fileMenu->addAction(saveOctScanAction);


	fileMenu->addSeparator();

	QAction* actionQuit = new QAction(this);
	actionQuit->setText(tr("Quit"));
	actionQuit->setIcon(QIcon(":/icons/door_in.png"));
	connect(actionQuit, &QAction::triggered, this, &OCTMarkerMainWindow::close);
	fileMenu->addAction(actionQuit);


	// ----------
	// Options
	// ----------
	QMenu* optionsMenu = new QMenu(this);
	optionsMenu->setTitle(tr("Options"));

	QAction* fillEpmtyPixelWhite = ProgramOptions::fillEmptyPixelWhite.getAction();
	QAction* registerBScans      = ProgramOptions::registerBScans     .getAction();
	QAction* loadRotateSlo       = ProgramOptions::loadRotateSlo      .getAction();
	QAction* holdOCTRawData      = ProgramOptions::holdOCTRawData     .getAction();
	QAction* readBScans          = ProgramOptions::readBScans         .getAction();
	fillEpmtyPixelWhite->setText(tr("Fill empty pixels white"));
	registerBScans     ->setText(tr("register BScans"));
	loadRotateSlo      ->setText(tr("rotate SLO"));
	holdOCTRawData     ->setText(tr("hold OCT raw data"));
	readBScans         ->setText(tr("read BScans from OCT data"));
	optionsMenu->addAction(fillEpmtyPixelWhite);
	optionsMenu->addAction(registerBScans);
	optionsMenu->addAction(loadRotateSlo );
	optionsMenu->addAction(holdOCTRawData);
	optionsMenu->addAction(readBScans);

	QMenu* optionsMenuE2E = new QMenu(this);
	optionsMenuE2E->setTitle(tr("E2E Gray"));
	optionsMenu->addMenu(optionsMenuE2E);

	QActionGroup* e2eGrayTransformGroup = new QActionGroup(this);
	static SendInt e2eGrayNativ(static_cast<int>(OctData::FileReadOptions::E2eGrayTransform::nativ));
	addMenuProgramOptionGroup(tr("E2E nativ"    ), ProgramOptions::e2eGrayTransform, optionsMenuE2E, e2eGrayNativ, e2eGrayTransformGroup, this);
	static SendInt e2eGrayXml   (static_cast<int>(OctData::FileReadOptions::E2eGrayTransform::xml));
	addMenuProgramOptionGroup(tr("Xml emulation"), ProgramOptions::e2eGrayTransform, optionsMenuE2E, e2eGrayXml  , e2eGrayTransformGroup, this);
	static SendInt e2eGrayVol   (static_cast<int>(OctData::FileReadOptions::E2eGrayTransform::vol));
	addMenuProgramOptionGroup(tr("Vol emulation"), ProgramOptions::e2eGrayTransform, optionsMenuE2E, e2eGrayVol  , e2eGrayTransformGroup, this);

	optionsMenu->addSeparator();



	QAction* autoSaveOctMarkers       = ProgramOptions::autoSaveOctMarkers.getAction();
	autoSaveOctMarkers->setText(tr("Autosave markers"));
	optionsMenu->addAction(autoSaveOctMarkers);


	QMenu* optionsMenuMarkersFileFormat = new QMenu(this);
	optionsMenuMarkersFileFormat->setTitle(tr("Default filetype"));
	optionsMenu->addMenu(optionsMenuMarkersFileFormat);

	QActionGroup* markersFileFormatGroup = new QActionGroup(this);
	static SendInt markerFFxml(OctMarkerIO::fileformat2Int(OctMarkerFileformat::XML));
	addMenuProgramOptionGroup(tr("XML" ), ProgramOptions::defaultFileformatOctMarkers, optionsMenuMarkersFileFormat, markerFFxml, markersFileFormatGroup, this);
	static SendInt markerFFjsom(OctMarkerIO::fileformat2Int(OctMarkerFileformat::Json));
	addMenuProgramOptionGroup(tr("JSOM"), ProgramOptions::defaultFileformatOctMarkers, optionsMenuMarkersFileFormat, markerFFjsom  , markersFileFormatGroup, this);
	static SendInt markerFFinfo(OctMarkerIO::fileformat2Int(OctMarkerFileformat::INFO));
	addMenuProgramOptionGroup(tr("INFO"), ProgramOptions::defaultFileformatOctMarkers, optionsMenuMarkersFileFormat, markerFFinfo  , markersFileFormatGroup, this);

	// ----------
	// View
	// ----------

	QMenu* viewMenu = new QMenu(this);
	viewMenu->setTitle(tr("View"));

	viewMenu->addAction(createColorOptionAction(ProgramOptions::bscanSegmetationLineColor, tr("change segmentation line color")));
	viewMenu->addSeparator();
	viewMenu->addAction(createColorOptionAction(ProgramOptions::layerSegActiveLineColor, tr("Active line color")));
	viewMenu->addAction(createColorOptionAction(ProgramOptions::layerSegPassivLineColor, tr("Passiv line color")));
	viewMenu->addSeparator();
	viewMenu->addAction(ProgramOptions::bscanShowExtraSegmentationslines.getAction());


	// ----------
	// Extras
	// ----------

	QMenu* extrisMenu = new QMenu(this);
	extrisMenu->setTitle(tr("Extras"));

	QAction* actionSaveMatlabBinCode = new QAction(this);
	actionSaveMatlabBinCode->setText(tr("Save Matlab Bin Code"));
	actionSaveMatlabBinCode->setIcon(QIcon(":/icons/disk.png"));
	connect(actionSaveMatlabBinCode, &QAction::triggered, this, &OCTMarkerMainWindow::saveMatlabBinCode);
	extrisMenu->addAction(actionSaveMatlabBinCode);

	QAction* actionSaveMatlabWriteBinCode = new QAction(this);
	actionSaveMatlabWriteBinCode->setText(tr("Save Matlab Write Bin Code"));
	actionSaveMatlabWriteBinCode->setIcon(QIcon(":/icons/disk.png"));
	connect(actionSaveMatlabWriteBinCode, &QAction::triggered, this, &OCTMarkerMainWindow::saveMatlabWriteBinCode);
	extrisMenu->addAction(actionSaveMatlabWriteBinCode);

	// ----------
	// Help Menu
	// ----------

	QMenu* helpMenu = new QMenu(this);
	helpMenu->setTitle(tr("Help"));

	QAction* aboutQtAct = new QAction(this);
	aboutQtAct->setText(tr("About Qt"));
    aboutQtAct->setStatusTip(tr("Show the Qt library's About box"));
	aboutQtAct->setIcon(QIcon(":/icons/help.png"));
	connect(aboutQtAct, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
	helpMenu->addAction(aboutQtAct);

	QAction* actionAboutDialog = new QAction(this);
	actionAboutDialog->setText(tr("About"));
	actionAboutDialog->setIcon(QIcon(":/icons/typicons/oct_marker_logo.svg"));
	connect(actionAboutDialog, SIGNAL(triggered()), this, SLOT(showAboutDialog()));
	helpMenu->addAction(actionAboutDialog);



	menuBar()->addMenu(fileMenu);
	menuBar()->addMenu(optionsMenu);
	menuBar()->addMenu(viewMenu);
	menuBar()->addMenu(extrisMenu);
	menuBar()->addMenu(helpMenu);


	//
	// Toolbar
	//

	QAction* previousOctScan = new QAction(this);
	previousOctScan->setText(tr("previous octScan"));
	previousOctScan->setIcon(QIcon(":/icons/resultset_previous.png"));
	previousOctScan->setShortcut(Qt::CTRL + Qt::LeftArrow);
	connect(previousOctScan, &QAction::triggered, &(OctFilesModel::getInstance()), &OctFilesModel::loadPreviousFile);

	QAction* nextOctScan = new QAction(this);
	nextOctScan->setText(tr("next octScan"));
	nextOctScan->setIcon(QIcon(":/icons/resultset_next.png"));
	nextOctScan->setShortcut(Qt::CTRL + Qt::RightArrow);
	connect(nextOctScan, &QAction::triggered, &(OctFilesModel::getInstance()), &OctFilesModel::loadNextFile);



	QAction* nextBScan = new QAction(this);
	nextBScan->setText(tr("next bscan"));
	nextBScan->setIcon(QIcon(":/icons/arrow_right.png"));
	nextBScan->setShortcut(Qt::RightArrow);
	connect(nextBScan, SIGNAL(triggered()), &markerManager, SLOT(nextBScan()));

	bscanChooser = new QSpinBox(this);
	connect(bscanChooser, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), &markerManager, &OctMarkerManager::chooseBScan);
// 	connect(bscanChooser, SIGNAL(valueChanged(int)), &markerManager, SLOT(chooseBScan(int)));
	connect(&markerManager, SIGNAL(bscanChanged(int)), bscanChooser, SLOT(setValue(int)));

	labelMaxBscan = new QLabel(this);
	labelMaxBscan->setText("/0");


	QAction* previousBScan = new QAction(this);
	previousBScan->setText(tr("previous bscan"));
	previousBScan->setIcon(QIcon(":/icons/arrow_left.png"));
	previousBScan->setShortcut(Qt::LeftArrow);
	connect(previousBScan, SIGNAL(triggered(bool)), &markerManager, SLOT(previousBScan()));

	QAction* extraSegLine = ProgramOptions::bscanShowExtraSegmentationslines.getAction();
	extraSegLine->setText(tr("show extra segmentationslines"));
	extraSegLine->setIcon(QIcon(":/icons/chart_curve.png"));

	QAction* showSeglines = ProgramOptions::bscansShowSegmentationslines.getAction();
	showSeglines->setText(tr("show segmentationslines"));
	showSeglines->setIcon(QIcon(":/icons/chart_curve.png"));

	QAction* showWithAspectRatio = ProgramOptions::bscanRespectAspectRatio.getAction();
	showWithAspectRatio->setText(tr("Aspect ratio"));
	showWithAspectRatio->setIcon(QIcon(":/icons/typicons/equals-outline.svg"));

	QSpinBox* paintsizeSegLines = new QSpinBox(this);
	paintsizeSegLines->setMinimum(1);
	paintsizeSegLines->setMaximum(8);
	paintsizeSegLines->setValue(ProgramOptions::bscanSegmetationLineThicknes());
	connect(paintsizeSegLines, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), &ProgramOptions::bscanSegmetationLineThicknes, &OptionInt::setValue);
	connect(&ProgramOptions::bscanSegmetationLineThicknes, &OptionInt::valueChanged, paintsizeSegLines, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::setValue));

	QToolBar* toolBar = new QToolBar(tr("B-Scan"));
	toolBar->setObjectName("ToolBarBScan");
	toolBar->addAction(previousOctScan);
	toolBar->addAction(nextOctScan);
	toolBar->addSeparator();
	toolBar->addAction(previousBScan);
	toolBar->addAction(nextBScan);
	toolBar->addSeparator();
	toolBar->addWidget(bscanChooser);
	toolBar->addWidget(labelMaxBscan);
	toolBar->addSeparator();
	toolBar->addAction(showWithAspectRatio);
	toolBar->addAction(extraSegLine);
	toolBar->addAction(showSeglines);
	toolBar->addWidget(paintsizeSegLines);



	toolBar->addSeparator();
	zoomInAction = new QAction(this);
	zoomInAction->setText(tr("Zoom +"));
	zoomInAction->setIcon(QIcon(":/icons/zoom_in.png"));
	connect(zoomInAction, &QAction::triggered, bscanMarkerWidget, &CVImageWidget::zoom_in);
	toolBar->addAction(zoomInAction);


	zoomMenu = new QMenu(tr("actual zoom"));
// 	zoomMenu->menuAction()->setIcon(QIcon(":/icons/zoom.png"));

	addZoomAction(1, bscanMarkerWidget, *zoomMenu);
	addZoomAction(2, bscanMarkerWidget, *zoomMenu);
	addZoomAction(3, bscanMarkerWidget, *zoomMenu);
	addZoomAction(4, bscanMarkerWidget, *zoomMenu);
	addZoomAction(6, bscanMarkerWidget, *zoomMenu);
	addZoomAction(8, bscanMarkerWidget, *zoomMenu);
// 	addZoomAction(10, bscanMarkerWidget, *zoomMenu);
// 	addZoomAction(12, bscanMarkerWidget, *zoomMenu);

	toolBar->addAction(zoomMenu->menuAction());




	zoomOutAction = new QAction(this);
	zoomOutAction->setText(tr("Zoom -"));
	zoomOutAction->setIcon(QIcon(":/icons/zoom_out.png"));
	connect(zoomOutAction, &QAction::triggered, bscanMarkerWidget, &CVImageWidget::zoom_out);
	toolBar->addAction(zoomOutAction);


	QAction* actionStrechBScanImage2MaxWidth = new QAction(this);
	actionStrechBScanImage2MaxWidth->setText(tr("Adjust image in width"));
	actionStrechBScanImage2MaxWidth->setIcon(QIcon(":/icons/stretch_width.png"));
	connect(actionStrechBScanImage2MaxWidth, &QAction::triggered, this, &OCTMarkerMainWindow::strechBScanImage2MaxWidth);
	toolBar->addAction(actionStrechBScanImage2MaxWidth);

	QAction* actionStrechBScanImage2MaxHeight = new QAction(this);
	actionStrechBScanImage2MaxHeight->setText(tr("Adjust image in height"));
	actionStrechBScanImage2MaxHeight->setIcon(QIcon(":/icons/stretch_height.png"));
	connect(actionStrechBScanImage2MaxHeight, &QAction::triggered, this, &OCTMarkerMainWindow::strechBScanImage2MaxHeight);
	toolBar->addAction(actionStrechBScanImage2MaxHeight);

	zoomChanged(bscanMarkerWidget->getScaleFactor());


	addToolBar(toolBar);
}

QAction * OCTMarkerMainWindow::createColorOptionAction(OptionColor& opt, const QString& text)
{
	QAction* colorAction = new QAction(this);
	colorAction->setText(text);
	colorAction->setIcon(QIcon(":/icons/color_wheel.png"));
	connect(colorAction, &QAction::triggered, &opt, &OptionColor::showColorDialog);
	return colorAction;
}


void OCTMarkerMainWindow::addZoomAction(int zoom, CVImageWidget* bscanMarkerWidget, QMenu& menue)
{
	IntValueAction* actionZoom = new IntValueAction(zoom, this, true);
	actionZoom->setText(tr("Zoom %1").arg(zoom));
	actionZoom->setIcon(QIcon(":/icons/zoom.png"));
	menue.addAction(actionZoom);
	connect(actionZoom       , &IntValueAction::triggered , bscanMarkerWidget, &CVImageWidget::setZoom      );
	connect(bscanMarkerWidget, &CVImageWidget::zoomChanged, actionZoom       , &IntValueAction::valueChanged);
}

void OCTMarkerMainWindow::setupStatusBar()
{

	OctDataManager& octDataManager = OctDataManager::getInstance();
	connect(&octDataManager, &OctDataManager::loadFileSignal  , this, &OCTMarkerMainWindow::loadFileStatusSlot);
	connect(&octDataManager, &OctDataManager::loadFileProgress, this, &OCTMarkerMainWindow::loadFileProgress  );

	loadProgressBar = new QProgressBar;
	loadProgressBar->setFixedWidth(200);
	loadProgressBar->setMinimum(0);
	loadProgressBar->setMaximum(100);
	loadProgressBar->setVisible(false);

	statusBar()->addPermanentWidget(loadProgressBar);

// 	MouseCoordStatus* mouseStatus = new MouseCoordStatus(bscanMarkerWidget);
// 	statusBar()->addPermanentWidget(mouseStatus);

}




void OCTMarkerMainWindow::zoomChanged(double zoom)
{
	if(zoomInAction ) zoomInAction ->setEnabled(zoom < 8);
	if(zoomOutAction) zoomOutAction->setEnabled(zoom > 0.5);

	if(zoomMenu)
	{
		QPixmap pixmap(24, 16);
		pixmap.fill(Qt::transparent);
		QPainter painter(&pixmap);
		QString string = QString::number(zoom, 'f', 1 );
		painter.drawText(0, 0, 24, 16, Qt::AlignHCenter | Qt::AlignVCenter, string);


		zoomMenu->menuAction()->setIcon(QIcon(pixmap));
	}
}

void OCTMarkerMainWindow::strechBScanImage2MaxWidth()
{
	bscanMarkerWidget->fitImage2Width(bscanMarkerWidgetScrollArea->width () - bscanMarkerWidgetScrollArea->getVScrollbarWidth () - 2);
}
void OCTMarkerMainWindow::strechBScanImage2MaxHeight()
{
	bscanMarkerWidget->fitImage2Height(bscanMarkerWidgetScrollArea->height() - bscanMarkerWidgetScrollArea->getHScrollbarHeight() - 2);
}




void OCTMarkerMainWindow::createMarkerToolbar()
{
	OctMarkerManager& markerManager = OctMarkerManager::getInstance();
	const std::vector<BscanMarkerBase*>& markers = markerManager.getBscanMarker();
	
	QToolBar*      toolBar            = new QToolBar(tr("Marker"));
	QActionGroup*  actionGroupMarker  = new QActionGroup(this);
	QSignalMapper* signalMapperMarker = new QSignalMapper(this);
	
	toolBar->setObjectName("ToolBarMarkerChoose");
// 	toolBar->setIconSize(QSize(60,60));

	QAction* markerAction = new QAction(this);
	markerAction->setCheckable(true);
	markerAction->setText(tr("no marker"));
	markerAction->setIcon(QIcon(":/icons/own/plain_retina.svg"));
	markerAction->setChecked(markerManager.getActBscanMarkerId() == -1);
	connect(markerAction, &QAction::triggered, signalMapperMarker, static_cast<void(QSignalMapper::*)()>(&QSignalMapper::map));
	signalMapperMarker->setMapping(markerAction, -1);
	actionGroupMarker->addAction(markerAction);
	toolBar->addAction(markerAction);
	
	int id = 0;
	for(BscanMarkerBase* marker : markers)
	{
		QAction* markerAction = new QAction(this);
		markerAction->setCheckable(true);
		markerAction->setText(marker->getName());
		markerAction->setIcon(marker->getIcon());
		markerAction->setChecked(markerManager.getActBscanMarkerId() == id);
		connect(markerAction, &QAction::triggered, signalMapperMarker, static_cast<void(QSignalMapper::*)()>(&QSignalMapper::map));
		signalMapperMarker->setMapping(markerAction, id);

		actionGroupMarker->addAction(markerAction);
		toolBar->addAction(markerAction);
		++id;
	}
	connect(signalMapperMarker, static_cast<void(QSignalMapper::*)(int)>(&QSignalMapper::mapped), &markerManager, &OctMarkerManager::setBscanMarker);
	
	addToolBar(toolBar);
}



void OCTMarkerMainWindow::showAboutDialog()
{
	InfoDialogs::showAboutDialog(this);
}



void OCTMarkerMainWindow::showImportFromFolderDialog()
{
	QFileDialog fd;
	// fd.selectFile(lineEditFile->text());
	fd.setWindowTitle(tr("Choose a folder to load Files"));
	fd.setAcceptMode(QFileDialog::AcceptOpen);
	fd.setFileMode(QFileDialog::Directory);
	if(fd.exec())
	{
		QStringList foldernames = fd.selectedFiles();
		loadFolder(foldernames[0]);
	}
}


void OCTMarkerMainWindow::showLoadImageDialog()
{
	QFileDialog fd;
	// fd.selectFile(lineEditFile->text());
	fd.setWindowTitle(tr("Choose a filename to load a File"));
	fd.setAcceptMode(QFileDialog::AcceptOpen);


	QStringList allExtentions;
	QStringList filtersOct;

	for(const OctData::OctExtension& ext : OctData::OctFileRead::supportedExtensions())
	{
		QStringList extensions;
		for(const std::string& str : ext.extensions)
		{
			if(!str.empty())
			{
				if(str[0] == '.')
				{
					extensions << QString("*%1").arg(str.c_str());
					allExtentions << QString("*%1").arg(str.c_str());
				}
				else
				{
					extensions << str.c_str();
					allExtentions << str.c_str();
				}
			}
		}
		// std::string extensions = boost::algorithm::join(ext.extensions, " ");
		filtersOct << QString("%1 (%2)").arg(ext.name.c_str()).arg(extensions.join(' '));
	}

	filtersOct.sort();
	
	QStringList filtersAllFiles;
	filtersAllFiles << tr("All readabel (%1)").arg(allExtentions.join(' '));
	filtersAllFiles << tr("All files (* *.*)");

	QStringList filters = filtersAllFiles + filtersOct;



	fd.setNameFilters(filters);
	fd.setFileMode(QFileDialog::ExistingFile);

	// fd.setDirectory("~/oct/");

	if(fd.exec())
	{
		QStringList filenames = fd.selectedFiles();
		loadFile(filenames[0]);
	}
}


void OCTMarkerMainWindow::handleOpenUrl(const QUrl& url, bool singleInput)
{
	QString filePath = url.toLocalFile();
	QFileInfo fileInfo = QFileInfo(filePath);
	if(fileInfo.isDir())
	{
		loadFolder(filePath);
	}
	if(OctData::OctFileRead::isLoadable(filePath.toStdString()))
	{
		if(singleInput)
			loadFile(filePath);
		else
			addFile(filePath);
	}
}


void OCTMarkerMainWindow::dropEvent(QDropEvent* event)
{
	const QMimeData* mimeData = event->mimeData();

	// check for our needed mime type, here a file or a list of files
	if(mimeData->hasUrls())
	{
		QStringList pathList;
		QList<QUrl> urlList = mimeData->urls();
		if(urlList.size() == 1)
		{
			handleOpenUrl(urlList.at(0), true);
		}
		else
		{
			for(const QUrl& url : urlList)
			{
				handleOpenUrl(url, false);
			}
		}
	}
}

void OCTMarkerMainWindow::dragEnterEvent(QDragEnterEvent* event)
{
	const QMimeData* mimeData = event->mimeData();
	if(mimeData->hasUrls())
	{
		QStringList pathList;
		QList<QUrl> urlList = mimeData->urls();
		for(const QUrl& url : urlList)
		{
			QString filePath = url.toLocalFile();
			QFileInfo fileInfo = QFileInfo(filePath);
			if(fileInfo.isDir())
			{
				event->acceptProposedAction();
				break;
			}
			if(OctData::OctFileRead::isLoadable(filePath.toStdString()))
			{
				event->acceptProposedAction();
				break;
			}
		}
	}
}

void OCTMarkerMainWindow::dragLeaveEvent(QDragLeaveEvent* event)
{
	QWidget::dragLeaveEvent(event);
}

void OCTMarkerMainWindow::dragMoveEvent(QDragMoveEvent* event)
{
	QWidget::dragMoveEvent(event);
}


bool OCTMarkerMainWindow::loadFile(const QString& filename)
{
	return OctFilesModel::getInstance().loadFile(filename);
}

bool OCTMarkerMainWindow::addFile(const QString& filename)
{
	return OctFilesModel::getInstance().addFile(filename);
}

std::size_t OCTMarkerMainWindow::loadFolder(const QString& foldername, int numMaxRecursiv)
{
	QDir folder(foldername);

	if(!folder.exists())
		return 0;

	OctFilesModel& filesModel = OctFilesModel::getInstance();

	std::size_t filesAdded = 0;

	const QStringList list = folder.entryList();

	for(int i = 0; i < list.size(); i++)
	{
		const QString& dirEntry = list[i];
		QString filePath = foldername + "/" + dirEntry;
		QFileInfo fileInfo = QFileInfo(filePath);
		// Skip  "." and ".."
		if(dirEntry == "." || dirEntry == "..")
			continue;
		if(fileInfo.isDir())
		{
			if(numMaxRecursiv > 0)
				filesAdded += loadFolder(filePath + "/", numMaxRecursiv-1);
		}
		else if(fileInfo.exists())
		{
			if(OctData::OctFileRead::isLoadable(filePath.toStdString()))
			{
				filesModel.addFile(filePath);
				++filesAdded;
			}
		}
	}
	return filesAdded;
}







/*

void OCTMarkerMainWindow::showAddMarkersDialog()
{
	QFileDialog fd;
	fd.setWindowTitle(tr("Choose a file to add markers"));
	fd.setAcceptMode(QFileDialog::AcceptOpen);

	fd.setNameFilter(tr("OCT Markers file (*_markers.xml)"));
	fd.setFileMode(QFileDialog::ExistingFile);

	if(fd.exec())
	{
		QStringList filenames = fd.selectedFiles();
		OctDataManager::getInstance().addMarkers(filenames[0], OctDataManager::Fileformat::XML);
	}
}
*/

void OCTMarkerMainWindow::setMarkersStringList(QStringList& filters)
{
	const char* josnExt = OctMarkerIO::getFileExtension(OctMarkerFileformat::Json);
	const char*  xmlExt = OctMarkerIO::getFileExtension(OctMarkerFileformat::XML);
	const char* infoExt = OctMarkerIO::getFileExtension(OctMarkerFileformat::INFO);
	
	filters << tr("OCT Markers")+QString(" (*.%1 *.%2 *.%3)").arg(josnExt).arg(xmlExt).arg(infoExt);
	filters << tr("OCT Markers Json file")+QString(" (*.%1)").arg(josnExt);
	filters << tr("OCT Markers XML file" )+QString(" (*.%1)").arg(xmlExt);
	filters << tr("OCT Markers INFO file")+QString(" (*.%1)").arg(infoExt);
}

namespace
{
	OctMarkerFileformat getMarkerFileFormat(const QString& filter)
	{
		QStringList filters;
		OCTMarkerMainWindow::setMarkersStringList(filters);
		int index = filters.indexOf(filter);
		
		static const OctMarkerFileformat formats[] = {OctMarkerFileformat::Json, OctMarkerFileformat::XML, OctMarkerFileformat::INFO};
		
		if(index == 0)
		   return OctMarkerFileformat::Auto;
		
		--index;
		if(index < static_cast<int>(sizeof(formats)/sizeof(formats[0])))
		{
			return formats[index];
		}
		
		throw "unknown filtername";
	}
}


void OCTMarkerMainWindow::setMarkersFilters(QFileDialog& fd)
{
	QStringList filters;
	setMarkersStringList(filters);
	fd.setNameFilters(filters);
}


void OCTMarkerMainWindow::showLoadMarkersDialog()
{
	QFileDialog fd;
	fd.setWindowTitle(tr("Choose a file to load markers"));
	fd.setAcceptMode(QFileDialog::AcceptOpen);

	setMarkersFilters(fd);
	fd.setFileMode(QFileDialog::ExistingFile);

	if(fd.exec())
	{
		QStringList filenames = fd.selectedFiles();
		OctDataManager::getInstance().loadMarkers(filenames[0], getMarkerFileFormat(fd.selectedNameFilter()));
	}
}

void OCTMarkerMainWindow::showSaveMarkersDialog()
{
	QFileDialog fd;
	fd.setWindowTitle(tr("Choose a filename to save markers"));
	fd.setAcceptMode(QFileDialog::AcceptSave);

	setMarkersFilters(fd);
	fd.setFileMode(QFileDialog::AnyFile);

	if(fd.exec())
	{
		QStringList filenames = fd.selectedFiles();
		OctDataManager::getInstance().saveMarkers(filenames[0], getMarkerFileFormat(fd.selectedNameFilter()));
	}
}


void OCTMarkerMainWindow::showSaveOctScanDialog()
{
	QFileDialog fd;
	QString filename = QFileDialog::getSaveFileName(this, tr("Choose a filename to save oct scan"), QString(), "*.octbin");
	if(!filename.isEmpty())
	{
		OctDataManager::getInstance().saveOctScan(filename);
	}
}

void OCTMarkerMainWindow::newCscanLoaded()
{
	setWindowTitle(tr("OCT-Marker - %1").arg(OctDataManager::getInstance().getLoadedFilename()));
	configBscanChooser();
}

void OCTMarkerMainWindow::configBscanChooser()
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

void OCTMarkerMainWindow::saveMatlabBinCode()
{
	QString filename = QFileDialog::getSaveFileName(this, tr("Save Matlab Bin Code"), QString("readbin.m"), "Matlab (*.m)");
	if(!filename.isEmpty())
		CppFW::CVMatTreeStructBin::writeMatlabReadCode(filename.toStdString().c_str());
}

void OCTMarkerMainWindow::saveMatlabWriteBinCode()
{
	QString filename = QFileDialog::getSaveFileName(this, tr("Save Matlab Write Bin Code"), QString("writebin.m"), "Matlab (*.m)");
	if(!filename.isEmpty())
		CppFW::CVMatTreeStructBin::writeMatlabWriteCode(filename.toStdString().c_str());
}

void OCTMarkerMainWindow::closeEvent(QCloseEvent* e)
{
	// save Markers
	bool saveSuccessful = true;
	std::string errorStr;
	try
	{
		OctDataManager::getInstance().saveMarkersDefault();
		if(!OctDataManager::getInstance().checkAndAskSaveBeforContinue())
			return e->ignore();
	}
	catch(boost::exception& e)
	{
		saveSuccessful = false;
		errorStr = boost::diagnostic_information(e);
	}
	catch(std::exception& e)
	{
		saveSuccessful = false;
		errorStr = e.what();
	}
	catch(const char* str)
	{
		saveSuccessful = false;
		errorStr = str;
	}
	catch(...)
	{
		saveSuccessful = false;
		errorStr = tr("Unknown error on autosave").toStdString();
	}
	if(!saveSuccessful)
	{
		int ret = QMessageBox::critical(this, tr("Error on autosave"), tr("Autosave fail with message: %1 <br />Quit program?").arg(errorStr.c_str()), QMessageBox::Yes | QMessageBox::No);
		if(ret == QMessageBox::No)
			return e->ignore();
	}



	// save programoptions
	ProgramOptions::loadOctdataAtStart.setValue(OctDataManager::getInstance().getLoadedFilename());
	
	ProgramOptions::writeAllOptions();

	QSettings& settings = ProgramOptions::getSettings();
	

	settings.setValue("mainWindowGeometry", saveGeometry());
	settings.setValue("mainWindowState"   , saveState()   );

	QWidget::closeEvent(e);
}


void SendInt::connectOptions(OptionInt& option, QAction* action)
{
	connect(action, &QAction::toggled, this   , &SendInt::recive    );
	connect(this  , &SendInt::send   , &option, &OptionInt::setValue);

	action->setChecked(option() == v);
}


void OCTMarkerMainWindow::loadFileStatusSlot(bool loading)
{
	if(loading)
	{
		loadProgressBar->setValue(0);
		setDisabled(true);
		loadProgressBar->setVisible(true);
	}
	else
	{
		setDisabled(false);
		loadProgressBar->setVisible(false);
	}
}


void OCTMarkerMainWindow::loadFileProgress(double frac)
{
	loadProgressBar->setValue(static_cast<int>(frac*100));
}


