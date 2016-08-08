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
#include <QLabel>

#include <widgets/dwsloimage.h>
#include <widgets/bscanmarkerwidget.h>
#include <octxmlread/octxml.h>
#include <data_structure/sloimage.h>
#include <data_structure/bscan.h>
#include <data_structure/intervallmarker.h>
#include <data_structure/cscan.h>
#include <manager/markermanager.h>

#include <boost/exception/exception.hpp>
#include <boost/exception/diagnostic_information.hpp>


namespace
{
	QIcon createColorIcon(const QColor& color)
	{
		QPixmap pixmap(15, 15);
		pixmap.fill(color);
		return QIcon(pixmap);
	}
}


OCTMarkerMainWindow::OCTMarkerMainWindow()
: QMainWindow()
, markerManager      (new MarkerManager      )
, dwSloImage         (new DWSloImage(*markerManager))
, bscanMarkerWidget  (new BScanMarkerWidget(*markerManager))
{
	addDockWidget(Qt::LeftDockWidgetArea, dwSloImage);

	setCentralWidget(bscanMarkerWidget);

	setWindowIcon(QIcon(":/icons/image_edit.png"));

	setupMenu();
	createMarkerToolbar();

	setActionToggel();

	try
	{
		markerManager->loadImage("testoct.xml");
	}
	catch(...) {}

}


OCTMarkerMainWindow::~OCTMarkerMainWindow()
{
	delete markerManager;
}



void OCTMarkerMainWindow::setupMenu()
{
	QMenu* fileMenu = new QMenu(this);
	fileMenu->setTitle(tr("File"));

	QAction* actionLoadImage = new QAction(this);
	actionLoadImage->setText(tr("Load OCT scan"));
	actionLoadImage->setShortcut(Qt::CTRL | Qt::Key_O);
	actionLoadImage->setIcon(QIcon(":/icons/folder.png"));
	connect(actionLoadImage, SIGNAL(triggered()), this, SLOT(showLoadImageDialog()));
	fileMenu->addAction(actionLoadImage);

	fileMenu->addSeparator();

	QAction* loadMarkersAction = new QAction(this);
	loadMarkersAction->setText(tr("load markers"));
	loadMarkersAction->setIcon(QIcon(":/icons/folder.png"));
	connect(loadMarkersAction, SIGNAL(triggered()), SLOT(showLoadMarkersDialog()));
	fileMenu->addAction(loadMarkersAction);


	QAction* addMarkersAction = new QAction(this);
	addMarkersAction->setText(tr("add markers"));
	addMarkersAction->setIcon(QIcon(":/icons/folder_add.png"));
	connect(addMarkersAction, SIGNAL(triggered()), SLOT(showAddMarkersDialog()));
	fileMenu->addAction(addMarkersAction);


	QAction* saveMarkersAction = new QAction(this);
	saveMarkersAction->setText(tr("save markers"));
	saveMarkersAction->setIcon(QIcon(":/icons/disk.png"));
	connect(saveMarkersAction, SIGNAL(triggered()), SLOT(showSaveMarkersDialog()));
	fileMenu->addAction(saveMarkersAction);

	fileMenu->addSeparator();

	QAction* actionQuit = new QAction(this);
	actionQuit->setText(tr("Quit"));
	actionQuit->setIcon(QIcon(":/icons/door_in.png"));
	connect(actionQuit, SIGNAL(triggered()), SLOT(close()));
	fileMenu->addAction(actionQuit);



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
	actionAboutDialog->setIcon(QIcon(":/icons/help.png"));
	connect(actionAboutDialog, SIGNAL(triggered()), this, SLOT(showAboutDialog()));
	helpMenu->addAction(actionAboutDialog);


	menuBar()->addMenu(fileMenu);
	menuBar()->addMenu(helpMenu);


	QAction* nextBScan = new QAction(this);
	nextBScan->setText(tr("next bscan"));
	nextBScan->setIcon(QIcon(":/icons/arrow_right.png"));
	nextBScan->setShortcut(Qt::RightArrow);
	connect(nextBScan, SIGNAL(triggered()), markerManager, SLOT(nextBScan()));

	bscanChooser = new QSpinBox(this);
	connect(bscanChooser, SIGNAL(valueChanged(int)), markerManager, SLOT(chooseBScan(int)));
	connect(markerManager, SIGNAL(bscanChanged(int)), bscanChooser, SLOT(setValue(int)));

	labelMaxBscan = new QLabel(this);
	labelMaxBscan->setText("/0");


	QAction* previousBScan = new QAction(this);
	previousBScan->setText(tr("previous bscan"));
	previousBScan->setIcon(QIcon(":/icons/arrow_left.png"));
	previousBScan->setShortcut(Qt::LeftArrow);
	connect(previousBScan, SIGNAL(triggered(bool)), markerManager, SLOT(previousBScan()));

	QToolBar* toolBar = new QToolBar(tr("B-Scan"));
	toolBar->addAction(previousBScan);
	toolBar->addAction(nextBScan);
	toolBar->addSeparator();
	toolBar->addWidget(bscanChooser);
	toolBar->addWidget(labelMaxBscan);

	addToolBar(toolBar);
}

void OCTMarkerMainWindow::createMarkerToolbar()
{
	QActionGroup*  actionGroupMarker  = new QActionGroup (this);
	QActionGroup*  actionGroupMethod  = new QActionGroup (this);
	QSignalMapper* signalMapperMarker = new QSignalMapper(this) ;
	QSignalMapper* signalMapperMethod = new QSignalMapper(this) ;
	QToolBar*      toolBar            = new QToolBar(tr("Marker"));

	const IntervallMarker& intervallMarker = IntervallMarker::getInstance();

	markersActions.resize(IntervallMarker::Marker::getMaxInternalId() + 1); // +1 for undefined marker

	for(const IntervallMarker::Marker& marker : intervallMarker.getIntervallMarkerList())
	{
		QIcon icon = createColorIcon(QColor::fromRgb(marker.getRed(), marker.getGreen(), marker.getBlue()));

		QAction* markerAction = new QAction(this);
		markerAction->setCheckable(true);
		markerAction->setText(QString::fromStdString(marker.getName()));
		markerAction->setIcon(icon);
		connect(markerAction, SIGNAL(triggered()), signalMapperMarker, SLOT(map()));
		signalMapperMarker->setMapping(markerAction, marker.getInternalId()) ;

		actionGroupMarker->addAction(markerAction);
		toolBar->addAction(markerAction);

		markersActions.at(marker.getInternalId()) = markerAction;
	}



	toolBar->addSeparator();


	paintMarkerAction = new QAction(this);
	paintMarkerAction->setCheckable(true);
	paintMarkerAction->setText(tr("paint marker"));
	paintMarkerAction->setIcon(QIcon(":/icons/paintbrush.png"));
	paintMarkerAction->setShortcut(Qt::LeftArrow);
	connect(paintMarkerAction, SIGNAL(triggered()), signalMapperMethod, SLOT(map()));
	signalMapperMethod->setMapping(paintMarkerAction, static_cast<int>(MarkerManager::Method::Paint));
	actionGroupMethod->addAction(paintMarkerAction);
	toolBar->addAction(paintMarkerAction);

	fillMarkerAction = new QAction(this);
	fillMarkerAction->setCheckable(true);
	fillMarkerAction->setText(tr("fill marker"));
	fillMarkerAction->setIcon(QIcon(":/icons/paintcan.png"));
	fillMarkerAction->setShortcut(Qt::LeftArrow);
	connect(fillMarkerAction, SIGNAL(triggered()), signalMapperMethod, SLOT(map()));
	signalMapperMethod->setMapping(fillMarkerAction, static_cast<int>(MarkerManager::Method::Fill));
	actionGroupMethod->addAction(fillMarkerAction);
	toolBar->addAction(fillMarkerAction);


	actionGroupMarker->setExclusive(true);
	actionGroupMethod->setExclusive(true);

	connect(signalMapperMarker, SIGNAL(mapped(int)), markerManager, SLOT(chooseMarkerID(int)));
	connect(signalMapperMethod, SIGNAL(mapped(int)), markerManager, SLOT(chooseMethodID(int)));

	addToolBar(toolBar);
}

void OCTMarkerMainWindow::setActionToggel()
{

	int markersId = markerManager->getActMarker().getInternalId();
	if(markersId >= 0)
		markersActions.at(markersId)->setChecked(true);

	switch(markerManager->getMarkerMethod())
	{
		case MarkerManager::Method::Paint:
			paintMarkerAction->setChecked(true);
			break;
		case MarkerManager::Method::Fill:
			fillMarkerAction->setChecked(true);
			break;
		default:
			qDebug("Undefined Action in %s %d", __FILE__, __LINE__);
	}
}




void OCTMarkerMainWindow::showAboutDialog()
{
	QString text("<h1><b>Info &#252;ber OCTMarker</b></h1>");

	// text += "<br />Dieses Programm entstand im Rahmen der Masterarbeit &#8222;Aktive-Konturen-Methoden zur Segmentierung von OCT-Aufnahmen&#8220;";
	text += "<br /><br />Author: Kay Gawlik";
	text += "<br /><br />Icons von <a href=\"http://www.famfamfam.com/lab/icons/silk/\">FamFamFam</a>";
	text += QString("<br /><br />&#220;bersetzungszeit: %1 %2").arg(__DATE__).arg(__TIME__);
	text += QString("<br />Qt-Version: %1").arg(qVersion());


	QMessageBox::about(this,tr("About"), text);
}


void OCTMarkerMainWindow::showLoadImageDialog()
{
	QFileDialog fd;
	// fd.selectFile(lineEditFile->text());
	fd.setWindowTitle(tr("Choose a filename to load a File"));
	fd.setAcceptMode(QFileDialog::AcceptOpen);

	QString allExtentions = "*.E2E *.vol *.xml *.sdb";
	
	QStringList filters;
	
#ifdef USE_DCMTK
	filters << tr("DICOM file (*.dcm *.DCM)");
	allExtentions += " *.dcm *.DCM";
#endif // USE_DCMTK
	
	filters << tr("All readabel (%1)").arg(allExtentions);
	filters << tr("All files (* *.*)").arg(allExtentions);
	filters << tr("OCT XML file (*.xml)");
	filters << tr("E2E file (*.E2E *.sdb)");
	filters << tr("VOL file (*.vol)");
	

	filters.sort();

	fd.setNameFilters(filters);
	fd.setFileMode(QFileDialog::ExistingFile);

	// fd.setDirectory("~/oct/");

	if(fd.exec())
	{
		QStringList filenames = fd.selectedFiles();

		try
		{
			markerManager->loadImage(filenames[0]);
		}
		catch(boost::exception& e)
		{
			QMessageBox msgBox;
			msgBox.setText(QString::fromStdString(boost::diagnostic_information(e)));
			msgBox.setIcon(QMessageBox::Critical);
			msgBox.exec();
		}
		catch(std::exception& e)
		{
			QMessageBox msgBox;
			msgBox.setText(QString::fromStdString(e.what()));
			msgBox.setIcon(QMessageBox::Critical);
			msgBox.exec();
		}
		catch(...)
		{
			QMessageBox msgBox;
			msgBox.setText(QString("Unknow error in file %1 line %2").arg(__FILE__).arg(__LINE__));
			msgBox.setIcon(QMessageBox::Critical);
			msgBox.exec();
		}

	}
}

void OCTMarkerMainWindow::showAddMarkersDialog()
{
	QFileDialog fd;
	fd.setWindowTitle(tr("Choose a file to add markers"));
	fd.setAcceptMode(QFileDialog::AcceptOpen);

	fd.setNameFilter(tr("OCT Markers file (*.xml)"));
	fd.setFileMode(QFileDialog::ExistingFile);

	if(fd.exec())
	{
		QStringList filenames = fd.selectedFiles();
		markerManager->addMarkersXml(filenames[0]);
	}
}

void OCTMarkerMainWindow::showLoadMarkersDialog()
{
	QFileDialog fd;
	fd.setWindowTitle(tr("Choose a file to load markers"));
	fd.setAcceptMode(QFileDialog::AcceptOpen);

	fd.setNameFilter(tr("OCT Markers file (*.xml)"));
	fd.setFileMode(QFileDialog::ExistingFile);

	if(fd.exec())
	{
		QStringList filenames = fd.selectedFiles();
		markerManager->loadMarkersXml(filenames[0]);
	}
}

void OCTMarkerMainWindow::showSaveMarkersDialog()
{
	QFileDialog fd;
	fd.setWindowTitle(tr("Choose a filename to save markers"));
	fd.setAcceptMode(QFileDialog::AcceptSave);

	fd.setNameFilter(tr("OCT Markers file (*.xml)"));
	fd.setFileMode(QFileDialog::AnyFile);

	if(fd.exec())
	{
		QStringList filenames = fd.selectedFiles();
		markerManager->saveMarkersXml(filenames[0]);
	}
}

void OCTMarkerMainWindow::newCscanLoaded()
{
	setWindowTitle(tr("OCT-Marker - %1").arg(markerManager->getFilename()));
	configBscanChooser();
}

void OCTMarkerMainWindow::configBscanChooser()
{
	int maxBscan = markerManager->getCScan().bscanCount()-1;
	bscanChooser->setMaximum(maxBscan);
	bscanChooser->setValue(markerManager->getActBScan());

	labelMaxBscan->setText(QString("/%1").arg(maxBscan));
}


