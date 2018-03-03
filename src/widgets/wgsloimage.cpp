#include "wgsloimage.h"

#include "slowithlegendwidget.h"

#include <QResizeEvent>
#include <QToolBar>
#include <QAction>
#include <QDockWidget>
#include <QSignalMapper>
#include <markermodules/slomarkerbase.h>

#include<helper/actionclasses.h>

#include<data_structure/programoptions.h>
#include<manager/octdatamanager.h>
#include<manager/octmarkermanager.h>

#include<widgets/doubleslider.h>

namespace
{
	void addIntAction(QToolBar* toolbar, OptionInt& intOption, int value, const QIcon& icon, const QString& text, WgSloImage* parent, QActionGroup* actionGroup = nullptr)
	{
		IntValueAction* intAction = new IntValueAction(value, parent);
		intAction->setText(text);
		intAction->setIcon(icon);
		intAction->setChecked(value == intOption());
		parent->connect(intAction, &IntValueAction::triggered, &intOption    , &OptionInt::setValue         );
		parent->connect(&intOption    , &OptionInt::valueChanged  , intAction, &IntValueAction::valueChanged);
		toolbar->addAction(intAction);
		if(actionGroup)
			actionGroup->addAction(intAction);
	}
}

WgSloImage::WgSloImage(QWidget* parent)
: QMainWindow(parent)
, markerManager(OctMarkerManager::getInstance())
, imageWidget(new SloWithLegendWidget(parent))
{
	setCentralWidget(imageWidget);


	QToolBar* bar = new QToolBar(this);

	bar->addAction(ProgramOptions::sloShowGrid.getAction());
	bar->addAction(ProgramOptions::sloShowBScanMousePos.getAction());
	bar->addAction(ProgramOptions::sloClipScanArea.getAction());

	bar->addSeparator();

	QActionGroup* actionGroupBscan = new QActionGroup(this);

	addIntAction(bar, ProgramOptions::sloShowsBScansPos, 0, QIcon(":/icons/image.png" ), tr("show no bscans"         ), this, actionGroupBscan);
	addIntAction(bar, ProgramOptions::sloShowsBScansPos, 1, QIcon(":/icons/layer.png" ), tr("show only actual bscans"), this, actionGroupBscan);
	addIntAction(bar, ProgramOptions::sloShowsBScansPos, 2, QIcon(":/icons/layers.png"), tr("show bscans"            ), this, actionGroupBscan);

	bar->addSeparator();
	bar->addAction(ProgramOptions::sloShowOverlay.getAction());

	DoubleSlider* overlayAlpha = new DoubleSlider(255, this);
	overlayAlpha->setRange(0., 1.);
	overlayAlpha->setOrientation(Qt::Horizontal);
	overlayAlpha->setValue(ProgramOptions::sloOverlayAlpha());
	bar->addWidget(overlayAlpha);
	connect(&ProgramOptions::sloOverlayAlpha, &OptionDouble::valueChanged, overlayAlpha                    , &DoubleSlider::setValue);
	connect(overlayAlpha                    , &DoubleSlider::valueChanged, &ProgramOptions::sloOverlayAlpha, &OptionDouble::setValue);


	setContextMenuPolicy(Qt::NoContextMenu);
	addToolBar(bar);

	createMarkerToolbar();

}

WgSloImage::~WgSloImage()
{
}



void WgSloImage::wheelEvent(QWheelEvent* wheelE)
{
	int deltaWheel = wheelE->delta();
	if(deltaWheel < 0)
		markerManager.previousBScan();
	else
		markerManager.nextBScan();
	wheelE->accept();
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

SLOImageWidget* WgSloImage::getImageWidget()
{
	return imageWidget->getImageWidget();
}
