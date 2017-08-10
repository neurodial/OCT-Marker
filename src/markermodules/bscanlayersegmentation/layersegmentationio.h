#ifndef LAYERSEGMENTATIONIO_H
#define LAYERSEGMENTATIONIO_H

#include<string>

class BScanLayerSegmentation;

class LayerSegmentationIO
{
public:
	static bool saveSegmentation2Bin(const BScanLayerSegmentation& marker, const std::string& filename);
};

#endif // LAYERSEGMENTATIONIO_H
