#include "wgsloimage.h"

#include "sloimagewidget.h"
#include <manager/markermanager.h>

#include <QResizeEvent>
#include <QToolBar>
#include <QAction>

#include <QGraphicsView>
#include <QGraphicsTextItem>

#include <markerobjects/rectitem.h>

WgSloImage::WgSloImage(MarkerManager& markerManger)
: QMainWindow(0)
, imageWidget(new SLOImageWidget(markerManger))
, markerManger(markerManger)
{

	imageWidget->setImageSize(size());
	setCentralWidget(imageWidget);


	gv = new QGraphicsView(this);
	gv->setStyleSheet("background: transparent");
	gv->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	gv->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	gv->setWindowFlags (Qt::FramelessWindowHint);
	gv->setCacheMode(QGraphicsView::CacheBackground);

	scene = new QGraphicsScene(this);
	gv->setScene(scene);

	RectItem* currentRect = new RectItem();
	currentRect->setDescription("ONH");
	currentRect->setRect(QRectF(50, 50, 50, 50));
	currentRect->setSelected(true);
	scene->addItem(currentRect);


	gv->show();
	gv->setGeometry(rect());


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
/*
	QPoint p = imageWidget->pos();
	QRect  r = imageWidget->rect();*/
	gv->setGeometry(imageWidget->geometry());
	// gv->setPos(imageWidget->pos());
	gv->resetTransform();
	gv->scale(imageWidget->getImageScaleFactor(), imageWidget->getImageScaleFactor());
	gv->setSceneRect(0, 0, imageWidget->imageWidth(), imageWidget->imageHight());
}
