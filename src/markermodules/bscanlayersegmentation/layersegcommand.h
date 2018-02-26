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
public:
	LayerSegCommand(BScanLayerSegmentation* parent, std::size_t start, std::vector<double>&& newPart, std::vector<double>&& oldPart);
	~LayerSegCommand();

	LayerSegCommand(const LayerSegCommand& other)            = delete;
	LayerSegCommand& operator=(const LayerSegCommand& other) = delete;


	virtual void apply();
	virtual void undo() ;
	virtual void redo() ;
};

#endif // LAYERSEGCOMMAND_H
