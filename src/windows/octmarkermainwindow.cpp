#include "octmarkermainwindow.h"

#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QToolBar>
#include <QFileDialog>
#include <QStringList>
#include <QSignalMapper>

#include <QActionGroup>

#include <widgets/dwsloimage.h>
#include <widgets/bscanmarkerwidget.h>
#include <octxmlread/octxml.h>
#include <data_structure/sloimage.h>
#include <data_structure/bscan.h>
#include <data_structure/intervallmarker.h>
#include <manager/markermanager.h>

#include <boost/exception/exception.hpp>
#include <boost/exception/diagnostic_information.hpp>


namespace
{
	QIcon createColorIcon(const QColor& color)
	{
		QPixmap pixmap(10,10);
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

	setupMenu();
	createMarkerToolbar();

	try
	{
		markerManager->loadOCTXml("testoct.xml");
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
	actionLoadImage->setText(tr("Load OCT scan (XML)"));
	actionLoadImage->setShortcut(Qt::CTRL | Qt::Key_O);
	actionLoadImage->setIcon(QIcon(":/icons/folder.png"));
	connect(actionLoadImage, SIGNAL(triggered()), this, SLOT(showLoadImageDialog()));
	fileMenu->addAction(actionLoadImage);


	fileMenu->addSeparator();

	QAction* actionQuit = new QAction(this);
	actionQuit->setText(tr("Quit"));
	actionQuit->setIcon(QIcon(":/icons/door_in.png"));
	connect(actionQuit, SIGNAL(triggered()), SLOT(close()));
	fileMenu->addAction(actionQuit);



	QMenu* helpMenu = new QMenu(this);
	helpMenu->setTitle(tr("Help"));

	QAction* actionAboutDialog = new QAction(this);
	actionAboutDialog->setText(tr("About"));
	actionAboutDialog->setIcon(QIcon(":/icons/help.png"));
	connect(actionAboutDialog, SIGNAL(triggered()), this, SLOT(showAboutDialog()));
	helpMenu->addAction(actionAboutDialog);



	menuBar()->addMenu(fileMenu);
	menuBar()->addMenu(helpMenu);


	QAction* nextBScan = new QAction(this);
	nextBScan->setText("next bscan");
	nextBScan->setIcon(QIcon(":/icons/arrow_right.png"));
	connect(nextBScan, SIGNAL(triggered()), markerManager, SLOT(nextBScan()));

	QAction* previousBScan = new QAction(this);
	previousBScan->setText("previous bscan");
	previousBScan->setIcon(QIcon(":/icons/arrow_left.png"));
	connect(previousBScan, SIGNAL(triggered(bool)), markerManager, SLOT(previousBScan()));

	QToolBar* toolBar = new QToolBar("B-Scan");
	toolBar->addAction(previousBScan);
	toolBar->addAction(nextBScan);

	addToolBar(toolBar);
}

void OCTMarkerMainWindow::createMarkerToolbar()
{
	QActionGroup*  actionGroup  = new QActionGroup (this);
	QSignalMapper* signalMapper = new QSignalMapper(this) ;

	QToolBar* toolBar = new QToolBar("Marker");

	const IntervallMarker& intervallMarker = IntervallMarker::getInstance();

	int counter = 0;
	for(const IntervallMarker::Marker& marker : intervallMarker.getIntervallMarkerList())
	{
		QIcon icon = createColorIcon(QColor::fromRgb(marker.getRed(), marker.getGreen(), marker.getBlue()));

		QAction* markerAction = new QAction(this);
		markerAction->setCheckable(true);
		markerAction->setText(QString::fromStdString(marker.getName()));
		markerAction->setIcon(icon);
		// connect(markerAction, SIGNAL(triggered()), markerManager, SLOT(nextBScan()));
		connect(markerAction, SIGNAL(triggered()), signalMapper, SLOT(map())) ;
		signalMapper->setMapping(markerAction, counter) ;

		actionGroup->addAction(markerAction);
		toolBar->addAction(markerAction);

		++counter;
	}


	connect(signalMapper, SIGNAL(mapped(int)), markerManager, SLOT(chooseMarkerID(int))) ;

	actionGroup->setExclusive(true);
	// toolBar->addAction(actionGroup);

	addToolBar(toolBar);
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

	QStringList filters;
	filters << "OCT XML file (*.xml)";

	filters.sort();
	fd.setNameFilters(filters);
	fd.setFileMode(QFileDialog::ExistingFile);

	// fd.setDirectory("~/oct/");

	if(fd.exec())
	{
		QStringList filenames = fd.selectedFiles();

		try
		{
			markerManager->loadOCTXml(filenames[0]);
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


