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

#ifndef WGINTERVALMARKER_H
#define WGINTERVALMARKER_H

#include <QWidget>

#include <vector>
#include <string>

class QToolBox;
class QButtonGroup;

class BScanIntervalMarker;
class IntervalMarker;

class WGIntervalMarker : public QWidget
{
	Q_OBJECT;

	BScanIntervalMarker* parent;
	QToolBox* toolboxCollections = nullptr;

	std::vector<std::string> collectionsInternalNames;
	std::vector<QButtonGroup*> collectionsButtonGroups;

	void addMarkerCollection(const IntervalMarker& markers);
	QWidget* createMarkerToolButtons();
	QWidget* createTransparencySlider();

public:
	WGIntervalMarker(BScanIntervalMarker* parent);
	~WGIntervalMarker();

private slots:
	void changeIntervalCollection(std::size_t index);
	void changeMarkerId(std::size_t index);
	void changeMarkerCollection(const std::string& internalName);


	void importMarkerFromBinSlot();
	void exportMarkerToBinSlot();
};

#endif // WGINTERVALMARKER_H
