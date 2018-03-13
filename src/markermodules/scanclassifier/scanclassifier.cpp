#include "scanclassifier.h"

#include"wgscanclassifier.h"

#include <octdata/datastruct/series.h>
#include <octdata/datastruct/bscan.h>



#include<iostream>

ScanClassifier::ScanClassifier(OctMarkerManager* markerManager)
: BscanMarkerBase(markerManager)
, scanClassifierStates(DefinedClassifierMarker::getInstance().getVolumeMarkers())
{
	name = tr("scan classifier");
	id   = "ScanClassifier";
	icon = QIcon(":/icons/tango/apps/accessories-text-editor.svgz");


	const DefinedClassifierMarker& definedClassifier = DefinedClassifierMarker::getInstance();

	for(const ClassifierMarker& classifier : definedClassifier.getVolumeMarkers())
	{
		ClassifierMarkerProxy* proxy = new ClassifierMarkerProxy(classifier);
		scanClassifierProxys.push_back(proxy);
	}

	for(const ClassifierMarker& classifier : definedClassifier.getBscanMarkers())
	{
		ClassifierMarkerProxy* proxy = new ClassifierMarkerProxy(classifier);
		slideClassifierProxys.push_back(proxy);
	}

	widgetPtr2WGScanClassifier = new WGScanClassifier(this);

}

ScanClassifier::~ScanClassifier()
{
	delete widgetPtr2WGScanClassifier;

	for(auto item : scanClassifierProxys)
		delete item;

	for(auto item : slideClassifierProxys)
		delete item;
}

void ScanClassifier::loadState(boost::property_tree::ptree& markerTree)
{
	BscanMarkerBase::loadState(markerTree);
	resetStates();
}

void ScanClassifier::saveState(boost::property_tree::ptree& markerTree)
{
	BscanMarkerBase::saveState(markerTree);
}

void ScanClassifier::newSeriesLoaded(const OctData::Series* series, boost::property_tree::ptree& ptree)
{
	BscanMarkerBase::newSeriesLoaded(series, ptree);
	slidesClassifierStates.resize(series->bscanCount(), DefinedClassifierMarker::getInstance().getBscanMarkers());
	resetStates();
}

void ScanClassifier::setActBScan(std::size_t bscan)
{
}

void ScanClassifier::resetStates()
{
	scanClassifierStates.reset();
	for(auto& itemList : slidesClassifierStates)
		itemList.reset();
}



ScanClassifier::ScanClassifierStates::ScanClassifierStates(const DefinedClassifierMarker::ClassifierMarkerMap& classifierMap)
{
	for(const ClassifierMarker& classifier : classifierMap)
		states.push_back(ClassifierMarkerState(classifier));
}

ScanClassifier::ScanClassifierStates::~ScanClassifierStates()
{
// 	for(ClassifierMarkerState* item : states)
// 		delete item;
}

void ScanClassifier::ScanClassifierStates::reset()
{
	for(ClassifierMarkerState& item : states)
		item.reset();
}
