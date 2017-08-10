#include "stupidsplinewindow.h"

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

#include <widgets/sloimagewidget.h>
#include <widgets/bscanmarkerwidget.h>
#include <widgets/wgoctdatatree.h>
#include <widgets/dwoctinformations.h>
#include <widgets/dwmarkerwidgets.h>
#include <widgets/scrollareapan.h>
#include <widgets/mousecoordstatus.h>
#include <widgets/dwdebugoutput.h>

#include <data_structure/intervalmarker.h>
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

#include <boost/algorithm/string/join.hpp>

#include <cpp_framework/cvmat/treestructbin.h>

#include <globaldefinitions.h>
#include <buildconstants.h>
#include <QWidgetAction>


#include <QPushButton>
#include <widgets/dwimagecoloradjustments.h>
#include <QToolButton>

#include<windows/infodialogs.h>


DWDebugOutput* StupidSplineWindow::dwDebugOutput = nullptr;


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

	protected:
		virtual void resizeEvent(QResizeEvent* event) override
		{
			imageWidget->setImageSize(event->size());
			QWidget::resizeEvent(event);
		}
	};

}


StupidSplineWindow::StupidSplineWindow(const char* filename)
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


	setWindowTitle(tr("OCT-Marker"));


	QSettings& settings = ProgramOptions::getSettings();
	restoreGeometry(settings.value("stupidMainWindowGeometry").toByteArray());

	
	// General Objects
	setCentralWidget(bscanMarkerWidgetScrollArea);


	QHBoxLayout* layoutZoomControl = new QHBoxLayout;

	QSize buttonSize(50, 50);

	QToolButton* infoButton = new QToolButton(this);
	infoButton->setIcon(QIcon(":/icons/question_mark_1.svg"));
	infoButton->setIconSize(buttonSize);
	connect(infoButton, &QToolButton::clicked, this, &StupidSplineWindow::showAboutDialog);
	layoutZoomControl->addWidget(infoButton);


	zoomInAction = new QAction(this);
	zoomInAction->setText(tr("Zoom +"));
	zoomInAction->setIcon(QIcon(":/icons/zoom_in_1.svg"));
	connect(zoomInAction, &QAction::triggered, bscanMarkerWidget, &CVImageWidget::zoom_in);
	QToolButton* buttonZoomIn = new QToolButton(this);
	buttonZoomIn->setDefaultAction(zoomInAction);
	buttonZoomIn->setIconSize(buttonSize);
	layoutZoomControl->addWidget(buttonZoomIn);

	zoomOutAction = new QAction(this);
	zoomOutAction->setText(tr("Zoom -"));
	zoomOutAction->setIcon(QIcon(":/icons/zoom_out_1.svg"));
	connect(zoomOutAction, &QAction::triggered, bscanMarkerWidget, &CVImageWidget::zoom_out);
	QToolButton* buttonZoomOut = new QToolButton(this);
	buttonZoomOut->setDefaultAction(zoomOutAction);
	buttonZoomOut->setIconSize(buttonSize);
	layoutZoomControl->addWidget(buttonZoomOut);


	QWidget* widgetZoomControl = new QWidget();
	widgetZoomControl->setLayout(layoutZoomControl);

	QDockWidget* dwZoomControl = new QDockWidget(this);
	dwZoomControl->setWindowTitle("SLO");
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


	OctMarkerManager& marker = OctMarkerManager::getInstance();
	marker.setBscanMarkerTextID(QString("LayerSegmentation"));

	DWMarkerWidgets* dwmarkerwidgets = new DWMarkerWidgets(this);
	dwmarkerwidgets->setObjectName("DwMarkerWidgets");
	dwmarkerwidgets->setFeatures(0);
	dwmarkerwidgets->setTitleBarWidget(new QWidget());
	addDockWidget(Qt::LeftDockWidgetArea, dwmarkerwidgets);



	QDockWidget* dwSaveAndClose = new QDockWidget(this);
	dwSaveAndClose->setFeatures(0);
	dwSaveAndClose->setWindowTitle("Quit");
	dwSaveAndClose->setObjectName("dwSaveAndClose");
	QPushButton* buttonSaveAndClose = new QPushButton(this);
	buttonSaveAndClose->setText("Save and Close");
	buttonSaveAndClose->setFont(QFont("Times", 24, QFont::Bold));

	dwSaveAndClose->setWidget(buttonSaveAndClose);
	dwSaveAndClose->setTitleBarWidget(new QWidget());
	addDockWidget(Qt::LeftDockWidgetArea, dwSaveAndClose);



	// General Config
	setWindowIcon(QIcon(":/icons/image_edit.png"));

	connect(bscanMarkerWidget, &CVImageWidget::zoomChanged, this, &StupidSplineWindow::zoomChanged);


	if(filename)
		loadFile(filename);
}

StupidSplineWindow::~StupidSplineWindow()
{
}

void StupidSplineWindow::messageOutput(QtMsgType type, const QMessageLogContext& context, const QString& msg)
{
	if(dwDebugOutput)
		dwDebugOutput->printMessages(type, context, msg);
}




void StupidSplineWindow::setupStatusBar()
{

	OctDataManager& octDataManager = OctDataManager::getInstance();
	connect(&octDataManager, &OctDataManager::loadFileSignal  , this, &StupidSplineWindow::loadFileStatusSlot);
	connect(&octDataManager, &OctDataManager::loadFileProgress, this, &StupidSplineWindow::loadFileProgress  );

	loadProgressBar = new QProgressBar;
	loadProgressBar->setFixedWidth(200);
	loadProgressBar->setMinimum(0);
	loadProgressBar->setMaximum(100);
	loadProgressBar->setVisible(false);

	statusBar()->addPermanentWidget(loadProgressBar);

	MouseCoordStatus* mouseStatus = new MouseCoordStatus(bscanMarkerWidget);
	statusBar()->addPermanentWidget(mouseStatus);

}




void StupidSplineWindow::zoomChanged(double zoom)
{
	if(zoomInAction ) zoomInAction ->setEnabled(zoom < 5);
	if(zoomOutAction) zoomOutAction->setEnabled(zoom > 1);
}


// void StupidSplineWindow::handleOpenUrl(const QUrl& url, bool singleInput)
// {
// 	QString filePath = url.toLocalFile();
// 	QFileInfo fileInfo = QFileInfo(filePath);
// 	if(fileInfo.isDir())
// 	{
// 		loadFolder(filePath);
// 	}
// 	if(OctData::OctFileRead::isLoadable(filePath.toStdString()))
// 	{
// 		if(singleInput)
// 			loadFile(filePath);
// 		else
// 			addFile(filePath);
// 	}
// }
//


// void StupidSplineWindow::setMarkersFilters(QFileDialog& fd)
// {
// 	QStringList filters;
// 	setMarkersStringList(filters);
// 	fd.setNameFilters(filters);
// }



void StupidSplineWindow::closeEvent(QCloseEvent* e)
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


void StupidSplineWindow::loadFileProgress(double frac)
{
	loadProgressBar->setValue(static_cast<int>(frac*100));
}



bool StupidSplineWindow::loadFile(const QString& filename)
{
	return OctFilesModel::getInstance().loadFile(filename);
}

void StupidSplineWindow::showAboutDialog()
{
	InfoDialogs::showAboutDialog(this);
}
