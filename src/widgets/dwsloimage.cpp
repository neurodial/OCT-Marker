#include "dwsloimage.h"

#include "sloimagewidget.h"
#include <manager/markermanager.h>
#include <QResizeEvent>

DWSloImage::DWSloImage(MarkerManager& markerManger)
: imageWidget(new SLOImageWidget(markerManger))
, markerManger(markerManger)
{
	setWidget(imageWidget);
	imageWidget->setImageSize(size());

	setWindowTitle(tr("SLO image"));

	// imageWidget->resize(100, imageWidget->height());
}


DWSloImage::~DWSloImage()
{
	delete imageWidget;
}


void DWSloImage::setSLOImage(const cv::Mat& image)
{
	imageWidget->showImage(image);
}

void DWSloImage::resizeEvent(QResizeEvent* event)
{
	QWidget::resizeEvent(event);

	imageWidget->setImageSize(event->size());
}

void DWSloImage::wheelEvent(QWheelEvent* wheelE)
{
	int deltaWheel = wheelE->delta();
	if(deltaWheel < 0)
		markerManger.previousBScan();
	else
		markerManger.nextBScan();
}

void DWSloImage::mouseReleaseEvent(QMouseEvent* mouseE)
{
	QWidget::mouseReleaseEvent(mouseE);
}
