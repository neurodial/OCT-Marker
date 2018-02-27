#include "octmarkeractions.h"

#include<QAction>
#include<QMenu>
#include<QPainter>

#include<manager/octmarkermanager.h>
#include<widgets/bscanmarkerwidget.h>
#include<windows/infodialogs.h>
#include<helper/actionclasses.h>

OctMarkerActions::OctMarkerActions(BScanMarkerWidget* bscanMarkerWidget)
: bscanMarkerWidget(bscanMarkerWidget)
{
	OctMarkerManager& markerManager = OctMarkerManager::getInstance();

	connect(&markerManager   , &OctMarkerManager::undoRedoStateChange, this, &OctMarkerActions::updateRedoUndo);
	connect(bscanMarkerWidget, &CVImageWidget::zoomChanged           , this, &OctMarkerActions::updateZoom    );
}


void OctMarkerActions::addZoomAction(int zoom, CVImageWidget* bscanMarkerWidget, QMenu& menue)
{
	IntValueAction* actionZoom = new IntValueAction(zoom, this, true);
	actionZoom->setText(tr("Zoom %1").arg(zoom));
	actionZoom->setIcon(QIcon(":/icons/zoom.png"));
	menue.addAction(actionZoom);
	connect(actionZoom       , &IntValueAction::triggered , bscanMarkerWidget, &CVImageWidget::setZoom      );
	connect(bscanMarkerWidget, &CVImageWidget::zoomChanged, actionZoom       , &IntValueAction::valueChanged);
}


QAction* OctMarkerActions::getZoomInAction()
{
	if(zoomInAction == nullptr)
	{
		zoomInAction = new QAction(this);
		zoomInAction->setText(tr("Zoom +"));
		zoomInAction->setIcon(QIcon::fromTheme("zoom-in",  QIcon(":/icons/typicons/zoom-in-outline.svg")));
		connect(zoomInAction, &QAction::triggered, bscanMarkerWidget, &CVImageWidget::zoom_in);
	}
	return zoomInAction;
}

QAction* OctMarkerActions::getZoomOutAction()
{
	if(zoomOutAction == nullptr)
	{
		zoomOutAction = new QAction(this);
		zoomOutAction->setText(tr("Zoom -"));
		zoomOutAction->setIcon(QIcon::fromTheme("zoom-out",  QIcon(":/icons/typicons/zoom-out-outline.svg")));
		connect(zoomOutAction, &QAction::triggered, bscanMarkerWidget, &CVImageWidget::zoom_out);
	}
	return zoomOutAction;
}


QAction* OctMarkerActions::getZoomMenuAction()
{
	if(!zoomMenu)
	{
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

		updateZoom(bscanMarkerWidget->getScaleFactor());
	}
	return zoomMenu->menuAction();
}

QAction* OctMarkerActions::getZoomFitAction()
{
	if(!zoomFitAction)
	{
		zoomFitAction = new QAction(this);
		zoomFitAction->setText(tr("fit image"));
		zoomFitAction->setIcon(QIcon::fromTheme("zoom-fit-best",  QIcon(":/icons/tango/actions/view-fullscreen.svgz")));
		connect(zoomFitAction, &QAction::triggered, bscanMarkerWidget, &CVImageWidget::fitImage2Parent);
	}
	return zoomFitAction;
}


QAction* OctMarkerActions::getZoomFitHeightAction()
{
	if(!zoomFitHeightAction)
	{
		zoomFitHeightAction = new QAction(this);
		zoomFitHeightAction->setText(tr("Adjust image in height"));
		zoomFitHeightAction->setIcon(QIcon(":/icons/stretch_height.png"));
		connect(zoomFitHeightAction, &QAction::triggered, bscanMarkerWidget, &CVImageWidget::fitImage2ParentHeight);
	}
	return zoomFitHeightAction;
}

QAction* OctMarkerActions::getZoomFitWidthAction()
{
	if(!zoomFitWidthAction)
	{
		zoomFitWidthAction = new QAction(this);
		zoomFitWidthAction->setText(tr("Adjust image in width"));
		zoomFitWidthAction->setIcon(QIcon(":/icons/stretch_width.png"));
		connect(zoomFitWidthAction, &QAction::triggered, bscanMarkerWidget, &CVImageWidget::fitImage2ParentWidth);
	}
	return zoomFitWidthAction;
}




QAction* OctMarkerActions::getRedoAction()
{
	if(!redoAction)
	{
		OctMarkerManager& markerManager = OctMarkerManager::getInstance();

		redoAction = new QAction(this);
		redoAction->setText(tr("redo"));
		redoAction->setIcon(QIcon::fromTheme("edit-redo", QIcon(":/icons/tango/actions/edit-redo.svgz")));
		redoAction->setShortcut(Qt::CTRL | Qt::SHIFT | Qt::Key_Z);
		redoAction->setEnabled(false);
		connect(redoAction, &QAction::triggered, &markerManager, &OctMarkerManager::callRedoStep);

	}
	return redoAction;
}

QAction* OctMarkerActions::getUndoAction()
{
	if(!undoAction)
	{
		OctMarkerManager& markerManager = OctMarkerManager::getInstance();

		undoAction = new QAction(this);
		undoAction->setText(tr("undo"));
		undoAction->setShortcut(Qt::CTRL | Qt::Key_Z);
		undoAction->setIcon(QIcon::fromTheme("edit-undo", QIcon(":/icons/tango/actions/edit-undo.svgz")));
		undoAction->setEnabled(false);
		connect(undoAction, &QAction::triggered, &markerManager, &OctMarkerManager::callUndoStep);
	}
	return undoAction;
}




QAction* OctMarkerActions::getAboutDialogAction()
{
	if(!aboutDialogAction)
	{
		aboutDialogAction = new QAction(this);
		aboutDialogAction->setText(tr("About OCT-Marker"));
		aboutDialogAction->setIcon(QIcon(":/icons/typicons/oct_marker_logo.svg"));
		connect(aboutDialogAction, &QAction::triggered, this, &OctMarkerActions::showAboutDialog);
	}
	return aboutDialogAction;
}





void OctMarkerActions::updateRedoUndo()
{
	OctMarkerManager& markerManager = OctMarkerManager::getInstance();

	if(undoAction)
		undoAction->setEnabled(markerManager.numUndoSteps() > 0);
	if(redoAction)
		redoAction->setEnabled(markerManager.numRedoSteps() > 0);
}




void OctMarkerActions::updateZoom(double zoom)
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


void OctMarkerActions::showAboutDialog()
{
	InfoDialogs::showAboutDialog(bscanMarkerWidget); // TODO: parent
}
