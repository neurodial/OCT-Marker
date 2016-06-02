#include "dwsloimage.h"

#include "sloimagewidget.h"
#include <QResizeEvent>

DWSloImage::DWSloImage(MarkerManager& markerManger)
: imageWidget(new SLOImageWidget(markerManger))
{
	setWidget(imageWidget);
	imageWidget->setImageSize(size());

	setWindowTitle(tr("SLO image"));

	imageWidget->resize(165, imageWidget->height());
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

