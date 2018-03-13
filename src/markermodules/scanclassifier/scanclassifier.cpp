#include "scanclassifier.h"

#include"wgscanclassifier.h"

#include <octdata/datastruct/series.h>
#include <octdata/datastruct/bscan.h>


ScanClassifier::ScanClassifier(OctMarkerManager* markerManager)
: BscanMarkerBase(markerManager)
, scanClassifierStates(DefinedClassifierMarker::getInstance().getVolumeMarkers())
, scanClassifierProxys(DefinedClassifierMarker::getInstance().getVolumeMarkers())
, slideClassifierProxys(DefinedClassifierMarker::getInstance().getBscanMarkers())
{
	name = tr("scan classifier");
	id   = "ScanClassifier";
	icon = QIcon(":/icons/tango/apps/accessories-text-editor.svgz");

	scanClassifierProxys.setClassifierStates(&scanClassifierStates);

	widgetPtr2WGScanClassifier = new WGScanClassifier(this);
}

ScanClassifier::~ScanClassifier()
{
	delete widgetPtr2WGScanClassifier;

}

void ScanClassifier::loadState(boost::property_tree::ptree& markerTree)
{
	resetStates();
	BscanMarkerBase::loadState(markerTree);
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

	setActBScan(getActBScanNr());
}

void ScanClassifier::setActBScan(std::size_t bscan)
{
	if(bscan < slidesClassifierStates.size())
		slideClassifierProxys.setClassifierStates(&slidesClassifierStates[bscan]);
	else
		slideClassifierProxys.setClassifierStates(nullptr);
}

void ScanClassifier::resetStates()
{
	scanClassifierStates.reset();
	for(auto& itemList : slidesClassifierStates)
		itemList.reset();
}



// State funktions

ScanClassifier::ClassifierStates::ClassifierStates(const DefinedClassifierMarker::ClassifierMarkerMap& classifierMap)
{
	for(const ClassifierMarker& classifier : classifierMap)
		states.push_back(ClassifierMarkerState(classifier));
}

ScanClassifier::ClassifierStates::~ClassifierStates()
{
}

void ScanClassifier::ClassifierStates::reset()
{
	for(ClassifierMarkerState& item : states)
		item.reset();
}



// Proxy funktions

ScanClassifier::ClassifierProxys::ClassifierProxys(const DefinedClassifierMarker::ClassifierMarkerMap& classifierMap)
{
	for(const ClassifierMarker& classifier : classifierMap)
	{
		ClassifierMarkerProxy* proxy = new ClassifierMarkerProxy(classifier);
		proxys.push_back(proxy);
	}
}

ScanClassifier::ClassifierProxys::~ClassifierProxys()
{
	for(ClassifierMarkerProxy* item : proxys)
		delete item;
}

void ScanClassifier::ClassifierProxys::setClassifierStates(ScanClassifier::ClassifierStates* states)
{
	if(states && states->size() == proxys.size())
	{
		for(std::size_t i = 0; i < proxys.size(); ++i)
			proxys[i]->setMarkerState(&(states->getState(i)));
	}
	else
	{
		for(ClassifierMarkerProxy* item : proxys)
			item->setMarkerState(nullptr);;
	}
}

