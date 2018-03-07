#include "scanclassifier.h"

#include"wgscanclassifier.h"

ScanClassifier::ScanClassifier(OctMarkerManager* markerManager)
: BscanMarkerBase(markerManager)
{
	name = tr("scan classifier");
	id   = "ScanClassifier";
	icon = QIcon(":/icons/tango/apps/accessories-text-editor.svgz");

	widgetPtr2WGScanClassifier = new WGScanClassifier(this);
}

ScanClassifier::~ScanClassifier()
{
	delete widgetPtr2WGScanClassifier;
}

void ScanClassifier::loadState(boost::property_tree::ptree& markerTree)
{
}

void ScanClassifier::saveState(boost::property_tree::ptree& markerTree)
{
}
