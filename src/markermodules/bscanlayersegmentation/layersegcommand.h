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

#ifndef LAYERSEGCOMMAND_H
#define LAYERSEGCOMMAND_H

#include <data_structure/markercommand.h>
#include<octdata/datastruct/segmentationlines.h>

class BScanLayerSegmentation;

class LayerSegCommand : public MarkerCommand
{
	BScanLayerSegmentation* const parent = nullptr;

	OctData::Segmentationlines::SegmentlineType type;
	std::size_t bscanNr;
	std::vector<double> newPart;
	std::vector<double> oldPart;
	std::size_t startPos;

	bool testAndChangeSegline();
public:
	LayerSegCommand(BScanLayerSegmentation* parent, std::size_t start, std::vector<double>&& newPart, std::vector<double>&& oldPart);
	~LayerSegCommand();

	LayerSegCommand(const LayerSegCommand& other)            = delete;
	LayerSegCommand& operator=(const LayerSegCommand& other) = delete;


	virtual void apply() override;
	virtual bool undo()  override;
	virtual bool redo()  override;
};

#endif // LAYERSEGCOMMAND_H
