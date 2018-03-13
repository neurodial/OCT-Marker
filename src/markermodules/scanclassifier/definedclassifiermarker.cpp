#include "definedclassifiermarker.h"

DefinedClassifierMarker::DefinedClassifierMarker()
{
	// ------------------------

	ClassifierMarker scanArea("scanArea", tr("scan area").toStdString(), ClassifierMarker::ClassifierChoiceType::Single);

	scanArea.addMarker(ClassifierMarker::Marker("macular", tr("macular").toStdString()));
	scanArea.addMarker(ClassifierMarker::Marker("onh"    , tr("onh"    ).toStdString()));

	addMarker(volumeMarkers, std::move(scanArea));


	// ------------------------

	ClassifierMarker scanAttributes("scanAttributes", "scan attributes", ClassifierMarker::ClassifierChoiceType::Multible);

	scanAttributes.addMarker(ClassifierMarker::Marker("attr1", "attribut 1"));
	scanAttributes.addMarker(ClassifierMarker::Marker("attr2", "attribut 2"));
	scanAttributes.addMarker(ClassifierMarker::Marker("attr3", "attribut 3"));
	scanAttributes.addMarker(ClassifierMarker::Marker("attr4", "attribut 4"));

	addMarker(volumeMarkers, std::move(scanAttributes));

	// ------------------------

	ClassifierMarker bscanAttributes("bscanAttributes", "b-scan attributes", ClassifierMarker::ClassifierChoiceType::Multible);

	bscanAttributes.addMarker(ClassifierMarker::Marker("attr1", "attribut A"));
	bscanAttributes.addMarker(ClassifierMarker::Marker("attr2", "attribut B"));
	bscanAttributes.addMarker(ClassifierMarker::Marker("attr3", "attribut C"));
	bscanAttributes.addMarker(ClassifierMarker::Marker("attr4", "attribut D"));

	addMarker(bscanMarkers, std::move(bscanAttributes));

	// ------------------------

	ClassifierMarker bscanAttributesSingle("bscanAttributesSingle", "b-scan attributes single", ClassifierMarker::ClassifierChoiceType::Single);

	bscanAttributesSingle.addMarker(ClassifierMarker::Marker("attr1", "attribut I"));
	bscanAttributesSingle.addMarker(ClassifierMarker::Marker("attr2", "attribut II"));
	bscanAttributesSingle.addMarker(ClassifierMarker::Marker("attr3", "attribut III"));
	bscanAttributesSingle.addMarker(ClassifierMarker::Marker("attr4", "attribut IV"));

	addMarker(bscanMarkers, std::move(bscanAttributesSingle));
}

void DefinedClassifierMarker::addMarker(ClassifierMarkerMap& map, ClassifierMarker&& marker)
{
// 	map.emplace(marker.getInternalName(), marker);
	map.emplace_back(marker);
}
