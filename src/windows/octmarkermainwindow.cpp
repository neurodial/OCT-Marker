#include "octmarkermainwindow.h"

#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>

#include <widgets/cvimagewidget.h>


OCTMarkerMainWindow::OCTMarkerMainWindow(): QMainWindow()
{

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
	// connect(actionLoadImage, SIGNAL(triggered()), workingImage, SLOT(showLoadImageDialog()));
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
