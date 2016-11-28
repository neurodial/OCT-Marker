#include "wgsloimage.h"

#include "sloimagewidget.h"
#include <manager/octmarkermanager.h>
#include <data_structure/programoptions.h>

#include <QResizeEvent>
#include <QToolBar>
#include <QAction>
#include <QSignalMapper>
#include <markermodules/slomarkerbase.h>

WgSloImage::WgSloImage(OctMarkerManager& markerManager, QWidget* parent)
: QMainWindow(parent)
, imageWidget(new SLOImageWidget(markerManager))
, markerManager(markerManager)
{

	imageWidget->setImageSize(size());
	setCentralWidget(imageWidget);

	QToolBar* bar = new QToolBar(this);

	QAction* showBScans = ProgramOptions::sloShowBscans.getAction();
	showBScans->setText(tr("show BScans"));
	showBScans->setIcon(QIcon(":/icons/layers.png"));
	connect(showBScans, &QAction::toggled, imageWidget, &SLOImageWidget::showBScans);
	bar->addAction(showBScans);

	QAction* showOnylActBScan = ProgramOptions::sloShowOnylActBScan.getAction();
	showOnylActBScan->setText(tr("show only actual BScans"));
	showOnylActBScan->setIcon(QIcon(":/icons/layers.png"));
	connect(showOnylActBScan, &QAction::toggled, imageWidget, &SLOImageWidget::showOnylActBScan);
	bar->addAction(showOnylActBScan);

	setContextMenuPolicy(Qt::NoContextMenu);
	addToolBar(bar);

	createMarkerToolbar();
}


void WgSloImage::wheelEvent(QWheelEvent* wheelE)
{
	int deltaWheel = wheelE->delta();
	if(deltaWheel < 0)
		markerManager.previousBScan();
	else
		markerManager.nextBScan();
}


void WgSloImage::resizeEvent(QResizeEvent* event)
{
	imageWidget->setImageSize(event->size());
	QWidget::resizeEvent(event);
}



void WgSloImage::createMarkerToolbar()
{
	const std::vector<SloMarkerBase*>& markers = markerManager.getSloMarker();

	QToolBar*      toolBar            = new QToolBar(tr("Marker"));
	QActionGroup*  actionGroupMarker  = new QActionGroup(this);
	QSignalMapper* signalMapperMarker = new QSignalMapper(this);

	toolBar->setObjectName("ToolBarMarkerChoose");

	QAction* markerAction = new QAction(this);
	markerAction->setCheckable(true);
	markerAction->setText(tr("no marker"));
	markerAction->setIcon(QIcon(":/icons/image.png"));
	markerAction->setChecked(markerManager.getActSloMarkerId() == -1);
	connect(markerAction, &QAction::triggered, signalMapperMarker, static_cast<void(QSignalMapper::*)()>(&QSignalMapper::map));
	signalMapperMarker->setMapping(markerAction, -1);
	actionGroupMarker->addAction(markerAction);
	toolBar->addAction(markerAction);

	int id = 0;
	for(SloMarkerBase* marker : markers)
	{
		QAction* markerAction = new QAction(this);
		markerAction->setCheckable(true);
		markerAction->setText(marker->getName());
		markerAction->setIcon(marker->getIcon());
		markerAction->setChecked(markerManager.getActSloMarkerId() == id);
		connect(markerAction, &QAction::triggered, signalMapperMarker, static_cast<void(QSignalMapper::*)()>(&QSignalMapper::map));
		signalMapperMarker->setMapping(markerAction, id);

		actionGroupMarker->addAction(markerAction);
		toolBar->addAction(markerAction);
		++id;
	}
	connect(signalMapperMarker, static_cast<void(QSignalMapper::*)(int)>(&QSignalMapper::mapped), &markerManager, &OctMarkerManager::setSloMarker);

	addToolBar(toolBar);
}
