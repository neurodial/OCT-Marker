#include "sloimagewidget.h"



SLOImageWidget::SLOImageWidget()
{
}

SLOImageWidget::~SLOImageWidget()
{
}

void SLOImageWidget::paintEvent(QPaintEvent* event)
{
	CVImageWidget::paintEvent(event);

	// Display the image
	QPainter painter(this);

	QBrush brush(QColor(255,0,0,80));

	// painter.setBrush(brush);
	painter.fillRect(25, 0, 20, height(), brush);

	painter.end();



}
