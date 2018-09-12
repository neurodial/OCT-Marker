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
