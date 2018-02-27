#include "octmarkeractions.h"

#include<QAction>

#include<manager/octmarkermanager.h>
#include<widgets/bscanmarkerwidget.h>

OctMarkerActions::OctMarkerActions(BScanMarkerWidget* bscanMarkerWidget)
: bscanMarkerWidget(bscanMarkerWidget)
{
	OctMarkerManager& markerManager = OctMarkerManager::getInstance();

	connect(&markerManager, &OctMarkerManager::undoRedoStateChange, this, &OctMarkerActions::updateRedoUndo);
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

QAction * OctMarkerActions::getUndoAction()
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

void OctMarkerActions::updateRedoUndo()
{
	OctMarkerManager& markerManager = OctMarkerManager::getInstance();

	if(undoAction)
		undoAction->setEnabled(markerManager.numUndoSteps() > 0);
	if(redoAction)
		redoAction->setEnabled(markerManager.numRedoSteps() > 0);
}

