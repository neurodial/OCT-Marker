#include "wgsloimage.h"

#include "sloimagewidget.h"
#include <manager/markermanager.h>

#include <QResizeEvent>
#include <QToolBar>
#include <QAction>

WgSloImage::WgSloImage(MarkerManager& markerManger)
: QMainWindow(0)
, imageWidget(new SLOImageWidget(markerManger))
, markerManger(markerManger)
{
	imageWidget->setImageSize(size());
	setCentralWidget(imageWidget);
	
	
	QToolBar* bar = new QToolBar(this);

	QAction* showBScans = new QAction(this);
	showBScans->setText(tr("show BScans"));
	showBScans->setIcon(QIcon(":/icons/help.png"));
	showBScans->setCheckable(true);
	showBScans->setChecked(imageWidget->getShowBScans());
	connect(showBScans, &QAction::toggled, imageWidget, &SLOImageWidget::showBScans);
	bar->addAction(showBScans);


	setContextMenuPolicy(Qt::NoContextMenu);
	addToolBar(bar);
}


void WgSloImage::wheelEvent(QWheelEvent* wheelE)
{
	int deltaWheel = wheelE->delta();
	if(deltaWheel < 0)
		markerManger.previousBScan();
	else
		markerManger.nextBScan();
}


void WgSloImage::resizeEvent(QResizeEvent* event)
{
	QWidget::resizeEvent(event);
	imageWidget->setImageSize(event->size());
}
