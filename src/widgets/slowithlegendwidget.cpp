#include"slowithlegendwidget.h"

#include<QGridLayout>


#include"sloimagewidget.h"
#include<manager/octmarkermanager.h>
#include<markermodules/bscanmarkerbase.h>


SloWithLegendWidget::SloWithLegendWidget(QWidget* parent)
: QWidget(parent)
, markerManager(OctMarkerManager::getInstance())
, imageWidget(new SLOImageWidget(this))
{
	layout = new QGridLayout(this);

	imageWidget->setImageSize(size());
	layout->addWidget(imageWidget);
	layout->setColumnStretch(0, 5);
	layout->setRowStretch(0, 5);
	layout->setContentsMargins(0,0,0,0);

	setLayout(layout);

	connect(&markerManager  , &OctMarkerManager::sloOverlayChanged , this, &SloWithLegendWidget::updateMarkerOverlayImage);
	connect(&markerManager  , &OctMarkerManager::bscanMarkerChanged, this, &SloWithLegendWidget::updateMarkerOverlayImage);
}


SloWithLegendWidget::~SloWithLegendWidget()
{
	if(actLegendWidget)
		layout->removeWidget(actLegendWidget);
}



void SloWithLegendWidget::updateMarkerOverlayImage()
{
	OctMarkerManager& markerManager = OctMarkerManager::getInstance();
	BscanMarkerBase* marker = markerManager.getActBscanMarker();

	if(actLegendWidget)
	{
		layout->removeWidget(actLegendWidget);
		actLegendWidget->hide();
		actLegendWidget = nullptr;
	}

	if(marker)
	{
		actLegendWidget = marker->getSloLegendWidget();
		if(actLegendWidget)
		{
			layout->addWidget(actLegendWidget, 0, 1);
			actLegendWidget->show();
		}
	}
}

