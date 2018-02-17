#pragma once

#include<QWidget>

class QGridLayout;
class OctMarkerManager;
class SLOImageWidget;

class SloWithLegendWidget : public QWidget
{
	Q_OBJECT

	OctMarkerManager& markerManager;
	SLOImageWidget* imageWidget;
	QWidget* actLegendWidget = nullptr;
	QGridLayout* layout = nullptr;

public:
	explicit SloWithLegendWidget(QWidget* parent);
	virtual ~SloWithLegendWidget();


	SLOImageWidget* getImageWidget() { return imageWidget; }

private slots:
	void updateMarkerOverlayImage();
};
