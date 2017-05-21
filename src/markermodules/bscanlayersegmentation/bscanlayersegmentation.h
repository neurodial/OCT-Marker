#ifndef BSCANLAYERSEGMENTATION_H
#define BSCANLAYERSEGMENTATION_H


#include "../bscanmarkerbase.h"

class BScanLayerSegmentation : public BscanMarkerBase
{
	Q_OBJECT
public:
	BScanLayerSegmentation(OctMarkerManager* markerManager);
};

#endif // BSCANLAYERSEGMENTATION_H
