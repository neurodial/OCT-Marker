#include "bscanlayersegmentation.h"

BScanLayerSegmentation::BScanLayerSegmentation(OctMarkerManager* markerManager)
: BscanMarkerBase(markerManager)
{
	name = tr("Layer Segmentation");
	id   = "layerSegmentation";
	icon = QIcon(":/icons/seglinelayer_edit.png");

}
