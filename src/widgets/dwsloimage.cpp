#include "dwsloimage.h"

#include "sloimagewidget.h"
#include "wgsloimage.h"
#include <manager/markermanager.h>

DWSloImage::DWSloImage(MarkerManager& markerManger)
: sloWidget(new WgSloImage(markerManger))
// : imageWidget(new SLOImageWidget(markerManger))
// , markerManger(markerManger)
{
	setWidget(sloWidget);
	// imageWidget->setImageSize(size());

	setWindowTitle(tr("SLO image"));

	// imageWidget->resize(100, imageWidget->height());
}


DWSloImage::~DWSloImage()
{
	delete sloWidget;
}
/*

void DWSloImage::setSLOImage(const cv::Mat& image)
{
	// imageWidget->showImage(image);
}*/

/*
void DWSloImage::resizeEvent(QResizeEvent* event)
{
	QWidget::resizeEvent(event);

	imageWidget->setImageSize(event->size());
}
*/
