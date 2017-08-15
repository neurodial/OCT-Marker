#include "mousecoordstatus.h"

#include "bscanmarkerwidget.h"

MouseCoordStatus::MouseCoordStatus(BScanMarkerWidget* bscanWidget)
: bscanWidget(bscanWidget)
{
	connect(bscanWidget, &BScanMarkerWidget::mouseLeaveImage, this, &MouseCoordStatus::mouseLeaveImage);
	connect(bscanWidget, &BScanMarkerWidget::mousePosInImage, this, &MouseCoordStatus::mousePosInImage);

	setTextFormat(Qt::PlainText);
}

MouseCoordStatus::~MouseCoordStatus()
{
}


void MouseCoordStatus::mouseLeaveImage()
{
	setText("");
}

void MouseCoordStatus::mousePosInImage(int x, int y)
{
	QString text;
	text.sprintf("X: %4d Y: %4d", x, y);
	setText(text);
}

