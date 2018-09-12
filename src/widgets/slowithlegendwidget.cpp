/*
 * Copyright (c) 2018 Kay Gawlik <kaydev@amarunet.de> <kay.gawlik@beuth-hochschule.de> <kay.gawlik@charite.de>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include"slowithlegendwidget.h"

#include<QGridLayout>


#include"sloimagewidget.h"
#include<manager/octmarkermanager.h>
#include<markermodules/bscanmarkerbase.h>
#include<markermodules/widgetoverlaylegend.h>


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
		WidgetOverlayLegend* overlaylegend = marker->getSloLegendWidget();
		if(overlaylegend)
		{
			actLegendWidget = overlaylegend->getWidget();
			if(actLegendWidget)
			{
				layout->addWidget(actLegendWidget, 0, 1);
				actLegendWidget->show();
			}
		}
	}
}

