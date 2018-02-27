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


void LayerSegCommand::apply()
{
}

void LayerSegCommand::undo()
{
	parent->modifiedSegPart(bscanNr, type, startPos, oldPart);
}

void LayerSegCommand::redo()
{
	parent->modifiedSegPart(bscanNr, type, startPos, newPart);
}
