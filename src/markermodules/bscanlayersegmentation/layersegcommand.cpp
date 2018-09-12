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

#include "layersegcommand.h"


#include "bscanlayersegmentation.h"

LayerSegCommand::LayerSegCommand(BScanLayerSegmentation* parent, std::size_t start, std::vector<double>&& newPart, std::vector<double>&& oldPart)
: parent(parent)
, type(parent->getActEditSeglineType())
, bscanNr(parent->getActBScanNr())
, newPart(std::move(newPart))
, oldPart(std::move(oldPart))
, startPos(start)
{
	MarkerCommand::bscan = static_cast<int>(bscanNr);
}


LayerSegCommand::~LayerSegCommand()
{
}

bool LayerSegCommand::testAndChangeSegline()
{
	if(parent->getActEditSeglineType() == type)
		return true;

	parent->setActEditLinetype(type);
	return false;
}


void LayerSegCommand::apply()
{
}

bool LayerSegCommand::undo()
{
	if(!testAndChangeSegline())
		return false;

	parent->modifiedSegPart(bscanNr, type, startPos, oldPart);
	return true;
}

bool LayerSegCommand::redo()
{
	if(!testAndChangeSegline())
		return false;

	parent->modifiedSegPart(bscanNr, type, startPos, newPart);
	return true;
}
