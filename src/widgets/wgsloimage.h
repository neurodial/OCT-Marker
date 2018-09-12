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

#ifndef WGSLOIMAGE_H
#define WGSLOIMAGE_H

#include <QMainWindow>


class OctMarkerManager;
class SloWithLegendWidget;
class SLOImageWidget;


class QGridLayout;


class WgSloImage : public QMainWindow
{
	Q_OBJECT
	
	OctMarkerManager& markerManager;
	SloWithLegendWidget* imageWidget;
	void createMarkerToolbar();
public:
	explicit WgSloImage(QWidget* parent = nullptr);
	~WgSloImage();
	
	SLOImageWidget* getImageWidget();

protected:
	virtual void wheelEvent       (QWheelEvent*);

};

#endif // WGSLOIMAGE_H
