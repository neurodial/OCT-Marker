#include "definedclassifiermarker.h"

DefinedClassifierMarker::DefinedClassifierMarker()
{

	ClassifierMarker scanArea("scanArea", tr("scan area").toStdString(), ClassifierMarker::ClassifierChoiceType::Single);

	scanArea.addMarker(ClassifierMarker::Marker("macular" , tr("macular").toStdString()));
	scanArea.addMarker(ClassifierMarker::Marker("onh"     , tr("onh"    ).toStdString()));

	addMarker(volumeMarkers, std::move(scanArea));

}

void DefinedClassifierMarker::addMarker(ClassifierMarkerMap& map, ClassifierMarker&& marker)
{
	map.emplace(marker.getInternalName(), marker);
}
