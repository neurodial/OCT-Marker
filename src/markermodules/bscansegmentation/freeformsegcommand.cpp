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

#include "freeformsegcommand.h"

#include<data_structure/simplecvmatcompress.h>

#include"bscansegmentation.h"

FreeFormSegCommand::FreeFormSegCommand(BScanSegmentation& parent, const SimpleCvMatCompress& mat)
: parent(parent)
, segmentationMat(new SimpleCvMatCompress(mat))
, bscanNr(parent.getActBScanNr())
{
	MarkerCommand::bscan = static_cast<int>(bscanNr);
}


FreeFormSegCommand::~FreeFormSegCommand()
{
	delete segmentationMat;
}


void FreeFormSegCommand::apply()
{

}

bool FreeFormSegCommand::undo()
{
	return parent.swapActMat(*segmentationMat);
}

bool FreeFormSegCommand::redo()
{
	return parent.swapActMat(*segmentationMat);
}
