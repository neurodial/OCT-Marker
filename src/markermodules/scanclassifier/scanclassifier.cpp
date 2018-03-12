#include "scanclassifier.h"

#include"wgscanclassifier.h"

#include"definedclassifiermarker.h"

ScanClassifier::ScanClassifier(OctMarkerManager* markerManager)
: BscanMarkerBase(markerManager)
{
	name = tr("scan classifier");
	id   = "ScanClassifier";
	icon = QIcon(":/icons/tango/apps/accessories-text-editor.svgz");


	const DefinedClassifierMarker& definedClassifier = DefinedClassifierMarker::getInstance();

	for(const ClassifierMarker& classifier : definedClassifier.getVolumeMarkers())
	{
		scanClassifierStates.push_back(new ClassifierMarkerState(classifier));

		ClassifierMarkerProxy* proxy = new ClassifierMarkerProxy(classifier);
		proxy->setMarkerState(scanClassifierStates.back());
		scanClassifierProxys.push_back(proxy);
	}

	widgetPtr2WGScanClassifier = new WGScanClassifier(this);

}

ScanClassifier::~ScanClassifier()
{
	delete widgetPtr2WGScanClassifier;

	for(auto item : scanClassifierStates)
		delete item;
	for(auto item : scanClassifierProxys)
		delete item;

}

void ScanClassifier::loadState(boost::property_tree::ptree& markerTree)
{
}

void ScanClassifier::saveState(boost::property_tree::ptree& markerTree)
{
}
