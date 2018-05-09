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
#include <QProgressBar>
#include <QStatusBar>
#include <QPushButton>

#include <widgets/wgsloimage.h>
#include <widgets/bscanmarkerwidget.h>
#include <widgets/wgoctdatatree.h>
#include <widgets/dwoctinformations.h>
#include <widgets/dwmarkerwidgets.h>
#include <widgets/scrollareapan.h>
#include <widgets/mousecoordstatus.h>
#include <widgets/dwdebugoutput.h>
#include <widgets/sloimagewidget.h>
#include <widgets/bscanchooserspinbox.h>

#include <data_structure/intervalmarker.h>
#include <data_structure/programoptions.h>

#include <manager/octmarkermanager.h>
#include <manager/octdatamanager.h>
#include <manager/octmarkerio.h>
#include <markermodules/bscanmarkerbase.h>

#include <model/octfilesmodel.h>
#include <model/octdatamodel.h>
#include <model/paintmarkermodel.h>

#include <octdata/datastruct/series.h>
#include <octdata/octfileread.h>
#include <octdata/filereadoptions.h>

#include <boost/exception/exception.hpp>
#include <boost/exception/diagnostic_information.hpp>

#include <boost/algorithm/string/join.hpp>

#include <cpp_framework/cvmat/treestructbin.h>

#include <globaldefinitions.h>

#include <widgets/dwimagecoloradjustments.h>

#include <widgets/wgoctmarkers.h>
#include <manager/paintmarker.h>

DWDebugOutput* OCTMarkerMainWindow::dwDebugOutput = nullptr;



OCTMarkerMainWindow::OCTMarkerMainWindow(bool loadLastFile)
: QMainWindow()
, dwSloImage       (new QDockWidget(this))
, bscanMarkerWidget(new BScanMarkerWidget)
, generalMarkerActions    (bscanMarkerWidget    )
{
	OctMarkerManager& markerManager = OctMarkerManager::getInstance();
	QSettings& settings = ProgramOptions::getSettings();
	restoreGeometry(settings.value("mainWindowGeometry").toByteArray());
	
	
	bscanMarkerWidgetScrollArea = new ScrollAreaPan(this);
	bscanMarkerWidgetScrollArea->setWidget(bscanMarkerWidget);
	connect(bscanMarkerWidget, &CVImageWidget::needScrollTo, bscanMarkerWidgetScrollArea, &ScrollAreaPan::scrollTo);

	generateMarkerActions();

	// General Objects
	setCentralWidget(bscanMarkerWidgetScrollArea);

// 	createActions();
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

#ifdef NDEBUG
	if(!dwDebugOutput)
		dwDebugOutput = new DWDebugOutput(this);
	dwDebugOutput->setObjectName("DWDebugOutput");
	addDockWidget(Qt::RightDockWidgetArea, dwDebugOutput);
#endif


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
	else
		std::cout << msg.toStdString() << std::endl;
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
	// ----------
	// fileMenu
	// ----------
	QMenu* fileMenu = new QMenu(this);
	fileMenu->setTitle(tr("File"));

	QAction* actionLoadImage = new QAction(this);
	actionLoadImage->setText(tr("Load OCT scan"));
	actionLoadImage->setShortcut(Qt::CTRL | Qt::Key_O);
	actionLoadImage->setIcon(QIcon::fromTheme("document-open", QIcon(":/icons/folder_image.png")));
	connect(actionLoadImage, &QAction::triggered, this, &OCTMarkerMainWindow::showLoadImageDialog);
	fileMenu->addAction(actionLoadImage);

	QAction* actionImportFolder = new QAction(this);
	actionImportFolder->setText(tr("Add OCT scans from folder"));
	actionImportFolder->setShortcut(Qt::CTRL | Qt::SHIFT | Qt::Key_O);
	actionImportFolder->setIcon(QIcon::fromTheme("folder-open", QIcon(":/icons/folder_add.png")));
	connect(actionImportFolder, &QAction::triggered, this, &OCTMarkerMainWindow::showImportFromFolderDialog);
	fileMenu->addAction(actionImportFolder);

	fileMenu->addSeparator();

	QAction* loadMarkersAction = new QAction(this);
	loadMarkersAction->setText(tr("load markers"));
	loadMarkersAction->setIcon(QIcon::fromTheme("document-open", QIcon(":/icons/folder.png")));
	connect(loadMarkersAction, &QAction::triggered, this, &OCTMarkerMainWindow::showLoadMarkersDialog);
	fileMenu->addAction(loadMarkersAction);

	QAction* saveMarkersAction = new QAction(this);
	saveMarkersAction->setText(tr("save markers"));
	saveMarkersAction->setIcon(QIcon::fromTheme("document-save-as", QIcon(":/icons/disk.png")));
	connect(saveMarkersAction, &QAction::triggered, this, &OCTMarkerMainWindow::showSaveMarkersDialog);
	fileMenu->addAction(saveMarkersAction);

	QAction* saveMarkersDefaultAction = new QAction(this);
	saveMarkersDefaultAction->setText(tr("trigger auto save markers"));
	saveMarkersDefaultAction->setIcon(QIcon::fromTheme("document-save", QIcon(":/icons/speichern.svg")));
	saveMarkersDefaultAction->setShortcut(Qt::CTRL | Qt::Key_S);
	connect(saveMarkersDefaultAction, &QAction::triggered, this, &OCTMarkerMainWindow::triggerSaveMarkersDefaultCatchErrors);
	fileMenu->addAction(saveMarkersDefaultAction);


	fileMenu->addSeparator();

	QAction* saveOctScanAction = new QAction(this);
	saveOctScanAction->setText(tr("save oct scan"));
	saveOctScanAction->setIcon(QIcon::fromTheme("document-save-as", QIcon(":/icons/tango/actions/document-save.svgz")));
	connect(saveOctScanAction, &QAction::triggered, this, &OCTMarkerMainWindow::showSaveOctScanDialog);
	fileMenu->addAction(saveOctScanAction);


	fileMenu->addSeparator();

	QAction* actionQuit = new QAction(this);
	actionQuit->setText(tr("Quit"));
	actionQuit->setIcon(QIcon::fromTheme("application-exit", QIcon(":/icons/tango/actions/system-log-out.svgz")));
	connect(actionQuit, &QAction::triggered, this, &OCTMarkerMainWindow::close);
	fileMenu->addAction(actionQuit);


	// ----------
	// View
	// ----------

	QMenu* layerSegmentMenu = new QMenu(this);
	layerSegmentMenu->setTitle(tr("Layer segmentation"));
	layerSegmentMenu->addAction(createColorOptionAction(ProgramOptions::layerSegActiveLineColor, tr("Active line color")));
	layerSegmentMenu->addAction(createColorOptionAction(ProgramOptions::layerSegPassivLineColor, tr("Passiv line color")));
	layerSegmentMenu->addAction(ProgramOptions::layerSegThicknessmapBlend.getAction());


	QMenu* viewMenu = new QMenu(this);
	viewMenu->setTitle(tr("View"));


	viewMenu->addAction(createColorOptionAction(ProgramOptions::bscanSegmetationLineColor, tr("change segmentation line color")));
	viewMenu->addSeparator();
	viewMenu->addMenu(layerSegmentMenu);
	viewMenu->addSeparator();
	viewMenu->addAction(ProgramOptions::bscanShowExtraSegmentationslines.getAction());


	// ----------
	// Extras
	// ----------
	QMenu* markerMenu = new QMenu(this);
	markerMenu->setTitle(tr("Marker"));

	for(QAction* markerAction : markerActions)
		markerMenu->addAction(markerAction);



	// ----------
	// Extras
	// ----------

	QMenu* extrasMenu = new QMenu(this);
	extrasMenu->setTitle(tr("Extras"));

	QAction* actionSaveMatlabBinCode = new QAction(this);
	actionSaveMatlabBinCode->setText(tr("Save Matlab Bin Code"));
	actionSaveMatlabBinCode->setIcon(QIcon::fromTheme("document-save-as", QIcon(":/icons/tango/actions/document-save.svgz")));
	connect(actionSaveMatlabBinCode, &QAction::triggered, this, &OCTMarkerMainWindow::saveMatlabBinCode);
	extrasMenu->addAction(actionSaveMatlabBinCode);

	QAction* actionSaveMatlabWriteBinCode = new QAction(this);
	actionSaveMatlabWriteBinCode->setText(tr("Save Matlab Write Bin Code"));
	actionSaveMatlabWriteBinCode->setIcon(QIcon::fromTheme("document-save-as", QIcon(":/icons/tango/actions/document-save.svgz")));
	connect(actionSaveMatlabWriteBinCode, &QAction::triggered, this, &OCTMarkerMainWindow::saveMatlabWriteBinCode);
	extrasMenu->addAction(actionSaveMatlabWriteBinCode);


	QAction* actionSaveScreenshot = new QAction(this);
	actionSaveScreenshot->setText(tr("create screenshot"));
	actionSaveScreenshot->setIcon(QIcon::fromTheme("document-save-as", QIcon(":/icons/tango/actions/document-save.svgz")));
	connect(actionSaveScreenshot, &QAction::triggered, this, &OCTMarkerMainWindow::screenshot);
	extrasMenu->addAction(actionSaveScreenshot);


	// ----------
	// Options
	// ----------
	QMenu* optionsMenu = new QMenu(this);
	optionsMenu->setTitle(tr("Options"));


	QMenu* optionsLoadOctMenu = new QMenu(this);
	optionsLoadOctMenu->setTitle(tr("Load options"));
	optionsMenu->addMenu(optionsLoadOctMenu);

	optionsLoadOctMenu->addAction(ProgramOptions::fillEmptyPixelWhite.getAction());
	optionsLoadOctMenu->addAction(ProgramOptions::registerBScans     .getAction());
	optionsLoadOctMenu->addAction(ProgramOptions::readBScans         .getAction());
	optionsLoadOctMenu->addAction(ProgramOptions::loadRotateSlo      .getAction());
	optionsLoadOctMenu->addAction(ProgramOptions::holdOCTRawData     .getAction());

	QMenu* optionsMenuE2E = new QMenu(this);
	optionsMenuE2E->setTitle(tr("E2E Gray"));
	optionsLoadOctMenu->addMenu(optionsMenuE2E);

	QActionGroup* e2eGrayTransformGroup = new QActionGroup(this);
	static SendInt e2eGrayNativ(static_cast<int>(OctData::FileReadOptions::E2eGrayTransform::nativ));
	addMenuProgramOptionGroup(tr("E2E nativ"    ), ProgramOptions::e2eGrayTransform, optionsMenuE2E, e2eGrayNativ, e2eGrayTransformGroup, this);
	static SendInt e2eGrayXml   (static_cast<int>(OctData::FileReadOptions::E2eGrayTransform::xml));
	addMenuProgramOptionGroup(tr("Xml emulation"), ProgramOptions::e2eGrayTransform, optionsMenuE2E, e2eGrayXml  , e2eGrayTransformGroup, this);
	static SendInt e2eGrayVol   (static_cast<int>(OctData::FileReadOptions::E2eGrayTransform::vol));
	addMenuProgramOptionGroup(tr("Vol emulation"), ProgramOptions::e2eGrayTransform, optionsMenuE2E, e2eGrayVol  , e2eGrayTransformGroup, this);
	static SendInt e2eGrayU16   (static_cast<int>(OctData::FileReadOptions::E2eGrayTransform::u16));
	addMenuProgramOptionGroup(tr("Vol emulation"), ProgramOptions::e2eGrayTransform, optionsMenuE2E, e2eGrayU16  , e2eGrayTransformGroup, this);


	optionsMenu->addAction(ProgramOptions::saveOctBinFlat     .getAction());
	optionsMenu->addSeparator();

	optionsMenu->addAction(ProgramOptions::autoSaveOctMarkers.getAction());


	QMenu* optionsMenuMarkersFileFormat = new QMenu(this);
	optionsMenuMarkersFileFormat->setTitle(tr("Default filetype"));
	optionsMenu->addMenu(optionsMenuMarkersFileFormat);

	QActionGroup* markersFileFormatGroup = new QActionGroup(this);
	static SendInt markerFFxml(OctMarkerIO::fileformat2Int(OctMarkerFileformat::XML));
	addMenuProgramOptionGroup(tr("XML" ), ProgramOptions::defaultFileformatOctMarkers, optionsMenuMarkersFileFormat, markerFFxml, markersFileFormatGroup, this);
	static SendInt markerFFjson(OctMarkerIO::fileformat2Int(OctMarkerFileformat::Json));
	addMenuProgramOptionGroup(tr("JSON"), ProgramOptions::defaultFileformatOctMarkers, optionsMenuMarkersFileFormat, markerFFjson  , markersFileFormatGroup, this);
	static SendInt markerFFinfo(OctMarkerIO::fileformat2Int(OctMarkerFileformat::INFO));
	addMenuProgramOptionGroup(tr("INFO"), ProgramOptions::defaultFileformatOctMarkers, optionsMenuMarkersFileFormat, markerFFinfo  , markersFileFormatGroup, this);


	// ----------
	// Help Menu
	// ----------

	QMenu* helpMenu = new QMenu(this);
	helpMenu->setTitle(tr("Help"));

	QAction* aboutQtAct = new QAction(this);
	aboutQtAct->setText(tr("About Qt"));
    aboutQtAct->setStatusTip(tr("Show the Qt library's About box"));
	aboutQtAct->setIcon(QIcon::fromTheme("dialog-information",  QIcon(":/icons/tango/apps/help-browser.svgz")));
	connect(aboutQtAct, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
	helpMenu->addAction(aboutQtAct);


	helpMenu->addAction(generalMarkerActions.getAboutDialogAction());



	menuBar()->addMenu(fileMenu);
	menuBar()->addMenu(markerMenu);
	menuBar()->addMenu(viewMenu);
	menuBar()->addMenu(extrasMenu);
	menuBar()->addMenu(optionsMenu);
	menuBar()->addMenu(helpMenu);


	//
	// Toolbar
	//

	QAction* previousOctScan = new QAction(this);
	previousOctScan->setText(tr("previous octScan"));
	previousOctScan->setIcon(QIcon::fromTheme("go-previous", QIcon(":/icons/tango/actions/go-previous.svgz")));
	previousOctScan->setShortcut(Qt::CTRL + Qt::LeftArrow);
	connect(previousOctScan, &QAction::triggered, &(OctFilesModel::getInstance()), &OctFilesModel::loadPreviousFile);

	QAction* nextOctScan = new QAction(this);
	nextOctScan->setText(tr("next octScan"));
	nextOctScan->setIcon(QIcon::fromTheme("go-next", QIcon(":/icons/tango/actions/go-next.svgz")));
	nextOctScan->setShortcut(Qt::CTRL + Qt::RightArrow);
	connect(nextOctScan, &QAction::triggered, &(OctFilesModel::getInstance()), &OctFilesModel::loadNextFile);


	QAction* extraSegLine        = ProgramOptions::bscanShowExtraSegmentationslines.getAction();
	QAction* showSeglines        = ProgramOptions::bscansShowSegmentationslines.getAction();
	QAction* showWithAspectRatio = ProgramOptions::bscanRespectAspectRatio.getAction();

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
	toolBar->addWidget(new BScanChooserSpinBox(this));
// 	toolBar->addWidget(labelMaxBscan);
	toolBar->addSeparator();
	toolBar->addAction(showWithAspectRatio);
	toolBar->addAction(extraSegLine);
	toolBar->addAction(showSeglines);
	toolBar->addWidget(paintsizeSegLines);



	toolBar->addSeparator();

	toolBar->addAction(generalMarkerActions.getZoomInAction());
	toolBar->addAction(generalMarkerActions.getZoomMenuAction());
	toolBar->addAction(generalMarkerActions.getZoomOutAction());
// 	toolBar->addAction(generalMarkerActions.getZoomFitAction());

	toolBar->addAction(ProgramOptions::bscanAutoFitImage.getAction());

	toolBar->addAction(generalMarkerActions.getZoomFitHeightAction());
	toolBar->addAction(generalMarkerActions.getZoomFitWidthAction());

	toolBar->addSeparator();
	toolBar->addAction(generalMarkerActions.getUndoAction());
	toolBar->addAction(generalMarkerActions.getRedoAction());

	addToolBar(toolBar);
}

QAction* OCTMarkerMainWindow::createColorOptionAction(OptionColor& opt, const QString& text)
{
	QAction* colorAction = new QAction(this);
	colorAction->setText(text);
	colorAction->setIcon(QIcon(":/icons/color_wheel.png"));
	connect(colorAction, &QAction::triggered, &opt, &OptionColor::showColorDialog);
	return colorAction;
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

void OCTMarkerMainWindow::generateMarkerActions()
{
	OctMarkerManager& markerManager = OctMarkerManager::getInstance();
	const std::vector<BscanMarkerBase*>& markers = markerManager.getBscanMarker();

	QActionGroup*  actionGroupMarker  = new QActionGroup(this);
	QSignalMapper* signalMapperMarker = new QSignalMapper(this);

	QAction* markerAction = new QAction(this);
	markerAction->setCheckable(true);
	markerAction->setText(tr("no marker"));
	markerAction->setIcon(QIcon(":/icons/own/plain_retina.svg"));
	markerAction->setChecked(markerManager.getActBscanMarkerId() == -1);
	connect(markerAction, &QAction::triggered, signalMapperMarker, static_cast<void(QSignalMapper::*)()>(&QSignalMapper::map));
	signalMapperMarker->setMapping(markerAction, -1);
	actionGroupMarker->addAction(markerAction);
	markerActions.push_back(markerAction);

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
		markerActions.push_back(markerAction);
		++id;
	}
	connect(signalMapperMarker, static_cast<void(QSignalMapper::*)(int)>(&QSignalMapper::mapped), &markerManager, &OctMarkerManager::setBscanMarker);
}


void OCTMarkerMainWindow::createMarkerToolbar()
{
	QToolBar* toolBar = new QToolBar(tr("Marker"));
	toolBar->setObjectName("markerToolbar");

	for(QAction* markerAction : markerActions)
		toolBar->addAction(markerAction);

	addToolBar(toolBar);
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
		const QList<QUrl> urlList = mimeData->urls();
		const bool loadFile = (urlList.size() == 1);
		for(const QUrl& url : urlList)
		{
			handleOpenUrl(url, loadFile);
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

bool OCTMarkerMainWindow::catchSaveError(std::function<void ()>& saveObj, std::string& errorStr, const QString& unknownErrorMessage = OCTMarkerMainWindow::tr("Unknown error on save"))
{
	bool saveSuccessful = true;
	try
	{
		saveObj();
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
		errorStr = unknownErrorMessage.toStdString();
	}
	return saveSuccessful;
}

void OCTMarkerMainWindow::showErrorDialog(bool isError, const std::string& errorMessage)
{
	if(isError)
		QMessageBox::critical(this, tr("Error on save"), tr("Save fail with message: %1").arg(errorMessage.c_str()), QMessageBox::Ok);
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
		if(filenames.size() > 0)
		{
			std::string errorStr;
			std::function<void ()> saveFun = [&]() { OctDataManager::getInstance().saveMarkers(filenames[0], getMarkerFileFormat(fd.selectedNameFilter())); };
			bool saveResult = catchSaveError(saveFun, errorStr);
			showErrorDialog(!saveResult, errorStr);
		}
	}
}


void OCTMarkerMainWindow::showSaveOctScanDialog()
{
	const QString& loadedFilename = OctDataManager::getInstance().getLoadedFilename();
	QFileInfo fileinfo(loadedFilename);
	QString filename = QFileDialog::getSaveFileName(this, tr("Choose a filename to save oct scan"), fileinfo.baseName()+".octbin", "*.octbin");
	if(!filename.isEmpty())
	{
		std::string errorStr;
		std::function<void ()> saveFun = [&]() { OctDataManager::getInstance().saveOctScan(filename); };
		bool saveResult = catchSaveError(saveFun, errorStr);
		showErrorDialog(!saveResult, errorStr);

	}
}

void OCTMarkerMainWindow::newCscanLoaded()
{
	setWindowTitle(tr("OCT-Marker - %1").arg(OctDataManager::getInstance().getLoadedFilename()));
}

void OCTMarkerMainWindow::saveMatlabBinCode()
{
	QString filename = QFileDialog::getSaveFileName(this, tr("Save Matlab Bin Code"), QString("readbin.m"), "Matlab (*.m)");
	if(!filename.isEmpty())
	{
		std::string errorStr;
		std::function<void ()> saveFun = [&]() { CppFW::CVMatTreeStructBin::writeMatlabReadCode(filename.toStdString().c_str()); };
		bool saveResult = catchSaveError(saveFun, errorStr);
		showErrorDialog(!saveResult, errorStr);
	}
}

void OCTMarkerMainWindow::saveMatlabWriteBinCode()
{
	QString filename = QFileDialog::getSaveFileName(this, tr("Save Matlab Write Bin Code"), QString("writebin.m"), "Matlab (*.m)");
	if(!filename.isEmpty())
	{
		std::string errorStr;
		std::function<void ()> saveFun = [&]() { CppFW::CVMatTreeStructBin::writeMatlabWriteCode(filename.toStdString().c_str()); };
		bool saveResult = catchSaveError(saveFun, errorStr);
		showErrorDialog(!saveResult, errorStr);
	}
}

void OCTMarkerMainWindow::closeEvent(QCloseEvent* e)
{
	// save Markers
	std::function<void ()> saveFun = [&]()
	{
		OctDataManager::getInstance().saveMarkersDefault();
		if(!OctDataManager::getInstance().checkAndAskSaveBeforContinue())
			return e->ignore();
	};

	std::string errorStr;
	bool saveSuccessful = catchSaveError(saveFun, errorStr, tr("Unknown error on autosave"));
	if(!saveSuccessful)
	{
		int ret = QMessageBox::critical(this, tr("Error on autosave"), tr("Autosave fail with message: %1 <br />Quit program?").arg(errorStr.c_str()), QMessageBox::Yes | QMessageBox::No);
		if(ret == QMessageBox::No)
			e->ignore();
	}
	if(!e->isAccepted())
		return;


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


void OCTMarkerMainWindow::screenshot()
{

	QString filename = QFileDialog::getSaveFileName(this, tr("save filename"), "octmarker.pdf", "*.pdf");
	if(filename.isEmpty())
		return;

	QPdfWriter generator(filename);
	generator.setTitle(tr("overlay legend"));;
	generator.setPageSize(QPageSize(size()));
	generator.setResolution(100);

	QPainter painter;
	QFont font;
	font.setPointSizeF(font.pointSizeF()*2);
	painter.begin(&generator);
	painter.setFont(font);
	render(&painter, QPoint(), QRegion(), DrawChildren | DrawWindowBackground | IgnoreMask);
// 				const QPoint &targetOffset = QPoint(), const QRegion &sourceRegion = QRegion(), RenderFlags renderFlags = RenderFlags( DrawWindowBackground | DrawChildren ))
	painter.end();

}

void OCTMarkerMainWindow::triggerSaveMarkersDefaultCatchErrors()
{
	std::string errorStr;
	std::function<void ()> saveFun = [&]() { OctDataManager::getInstance().triggerSaveMarkersDefault(); };
	bool saveResult = catchSaveError(saveFun, errorStr);
	showErrorDialog(!saveResult, errorStr);
}
