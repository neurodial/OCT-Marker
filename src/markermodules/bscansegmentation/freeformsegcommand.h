/*
 * <one line to give the program's name and a brief idea of what it does.>
 * Copyright (C) 2018  kay
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
 */

#ifndef FREEFORMSEGCOMMAND_H
#define FREEFORMSEGCOMMAND_H

#include<cstddef>

#include<data_structure/markercommand.h>

class BScanSegmentation;
class SimpleCvMatCompress;

class FreeFormSegCommand : public MarkerCommand
{
	BScanSegmentation& parent;
	SimpleCvMatCompress* segmentationMat;

	std::size_t bscanNr;

public:
	FreeFormSegCommand(BScanSegmentation& parent, const SimpleCvMatCompress& mat);
	~FreeFormSegCommand();

	FreeFormSegCommand(const FreeFormSegCommand &other)            = delete;
	FreeFormSegCommand &operator=(const FreeFormSegCommand &other) = delete;


	virtual void apply();
	virtual bool undo();
	virtual bool redo();
};

#endif // FREEFORMSEGCOMMAND_H
