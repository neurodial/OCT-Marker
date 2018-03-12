#include "definedclassifiermarker.h"

DefinedClassifierMarker::DefinedClassifierMarker()
{

	ClassifierMarker scanArea("scanArea", tr("scan area").toStdString(), ClassifierMarker::ClassifierChoiceType::Single);

	scanArea.addMarker(ClassifierMarker::Marker("macular", tr("macular").toStdString()));
	scanArea.addMarker(ClassifierMarker::Marker("onh"    , tr("onh"    ).toStdString()));

	addMarker(volumeMarkers, std::move(scanArea));


	ClassifierMarker scanAttributes("scanAttributes", tr("scan attributes").toStdString(), ClassifierMarker::ClassifierChoiceType::Multible);

	scanAttributes.addMarker(ClassifierMarker::Marker("attr1", "attribut 1"));
	scanAttributes.addMarker(ClassifierMarker::Marker("attr2", "attribut 2"));
	scanAttributes.addMarker(ClassifierMarker::Marker("attr3", "attribut 3"));
	scanAttributes.addMarker(ClassifierMarker::Marker("attr4", "attribut 4"));

	addMarker(volumeMarkers, std::move(scanAttributes));

}

void DefinedClassifierMarker::addMarker(ClassifierMarkerMap& map, ClassifierMarker&& marker)
{
// 	map.emplace(marker.getInternalName(), marker);
	map.emplace_back(marker);
}
