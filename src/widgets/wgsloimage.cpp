#include "wgsloimage.h"

#include "sloimagewidget.h"
#include <manager/markermanager.h>

#include <QResizeEvent>
#include <QToolBar>

WgSloImage::WgSloImage(MarkerManager& markerManger)
: QMainWindow(0)
, imageWidget(new SLOImageWidget(markerManger))
, markerManger(markerManger)
{
	imageWidget->setImageSize(size());
	setCentralWidget(imageWidget);
	
	
	QToolBar* bar = new QToolBar(this);
	bar->addAction("First");
	// bar->clo(false);
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
