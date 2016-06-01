#include "octmarkermainwindow.h"

#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QToolBar>

#include <widgets/cvimagewidget.h>
#include <octxmlread/octxml.h>
#include <data_structure/cscan.h>
#include <data_structure/sloimage.h>
#include <data_structure/bscan.h>


OCTMarkerMainWindow::OCTMarkerMainWindow()
: QMainWindow()
, cscan(new CScan)
{

	imageWidget = new CVImageWidget(this);
	setCentralWidget(imageWidget);

	setupMenu();

}


OCTMarkerMainWindow::~OCTMarkerMainWindow()
{

}



void OCTMarkerMainWindow::setupMenu()
{
	QMenu* fileMenu = new QMenu(this);
	fileMenu->setTitle(tr("File"));

	QAction* actionLoadImage = new QAction(this);
	actionLoadImage->setText(tr("Load OCT scan (XML)"));
	actionLoadImage->setShortcut(Qt::CTRL | Qt::Key_O);
	actionLoadImage->setIcon(QIcon(":/icons/folder_image.png"));
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
	actionAboutDialog->setIcon(QIcon(":/icons/eye.png"));
	connect(actionAboutDialog, SIGNAL(triggered()), this, SLOT(showAboutDialog()));
	helpMenu->addAction(actionAboutDialog);



	menuBar()->addMenu(fileMenu);
	menuBar()->addMenu(helpMenu);


	QAction* nextBScan = new QAction(this);
	nextBScan->setText("->");
	connect(nextBScan, SIGNAL(triggered()), this, SLOT(nextBScan()));

	QAction* previousBScan = new QAction(this);
	previousBScan->setText("<-");
	connect(previousBScan, SIGNAL(triggered(bool)), this, SLOT(previousBScan()));

	QToolBar* toolBar = new QToolBar("B-Scan");
	toolBar->addAction(previousBScan);
	toolBar->addAction(nextBScan);

	addToolBar(toolBar);
}

void OCTMarkerMainWindow::showAboutDialog()
{
	QString text("<h1><b>Info &#252;ber OCTMarker</b></h1>");

	// text += "<br />Dieses Programm entstand im Rahmen der Masterarbeit &#8222;Aktive-Konturen-Methoden zur Segmentierung von OCT-Aufnahmen&#8220;";
	text += "<br /><br />Author: Kay Gawlik";
	// text += "<br /><br />Icons von <a href=\"http://www.famfamfam.com/lab/icons/silk/\">FamFamFam</a>";
	text += QString("<br /><br />&#220;bersetzungszeit: %1 %2").arg(__DATE__).arg(__TIME__);
	text += QString("<br />Qt-Version: %1").arg(qVersion());


	QMessageBox::about(this,tr("About"), text);
}


void OCTMarkerMainWindow::showLoadImageDialog()
{
	delete cscan;
	cscan = new CScan;

	OctXml::readOctXml("testoct.xml", cscan);

	bscanPos = 0;

	showBScan();
// 	const SLOImage* image = cscan->getSloImage();
// 	if(image)
// 		imageWidget->showImage(image->getImage());
}


void OCTMarkerMainWindow::showBScan()
{
	const BScan* bscan = cscan->getBScan(bscanPos);
	if(bscan)
		imageWidget->showImage(bscan->getImage());
}


void OCTMarkerMainWindow::nextBScan()
{
	if(bscanPos < cscan->bscanCount()-1)
	{
		++bscanPos;
		showBScan();
	}
}

void OCTMarkerMainWindow::previousBScan()
{
	if(bscanPos > 0)
	{
		--bscanPos;
		showBScan();
	}
}

