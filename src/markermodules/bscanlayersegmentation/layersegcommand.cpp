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
