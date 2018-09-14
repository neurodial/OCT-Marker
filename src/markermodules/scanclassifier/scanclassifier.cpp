/*
 * Copyright (c) 2018 Kay Gawlik <kaydev@amarunet.de> <kay.gawlik@beuth-hochschule.de> <kay.gawlik@charite.de>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "scanclassifier.h"


#include <boost/property_tree/ptree.hpp>
#include <boost/lexical_cast.hpp>
namespace bpt = boost::property_tree;
#include<helper/ptreehelper.h>

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

	widgetPtr2WGScanClassifier = new WGScanClassifier(this);
}

ScanClassifier::~ScanClassifier()
{
	delete widgetPtr2WGScanClassifier;

}

void ScanClassifier::loadBScansState(const boost::property_tree::ptree& markerTree)
{
	boost::optional<const bpt::ptree&> bscansNode = markerTree.get_child_optional("BScans");
	if(!bscansNode)
		return;

	for(const std::pair<const std::string, const bpt::ptree>& bscanPair : *bscansNode)
	{
		if(bscanPair.first != "BScan")
			continue;

		const bpt::ptree& bscanNode = bscanPair.second;
		boost::optional<const bpt::ptree&> idNodeOpt = bscanNode.get_child_optional("ID");
		if(!idNodeOpt)
			continue;
		int bscanId = idNodeOpt->get_value<int>(-1);
		if(bscanId == -1 || static_cast<std::size_t>(bscanId) >= slidesClassifierStates.size())
			continue;

		slidesClassifierStates[bscanId].loadState(bscanNode);
	}
}


void ScanClassifier::loadState(boost::property_tree::ptree& markerTree)
{
	resetStates();
	BscanMarkerBase::loadState(markerTree);

	boost::optional<bpt::ptree&> scanNodeOpt =  markerTree.get_child_optional("Scan");
	if(scanNodeOpt)
		scanClassifierStates.loadState(*scanNodeOpt);

	loadBScansState(markerTree);

	updateStateProxys();
}

void ScanClassifier::saveState(boost::property_tree::ptree& markerTree)
{
	BscanMarkerBase::saveState(markerTree);

	bpt::ptree& scanTree = PTreeHelper::get_put(markerTree, "Scan");
	scanClassifierStates.saveState(scanTree);

	bpt::ptree& bscansTree = PTreeHelper::get_put(markerTree, "BScans");
	for(std::size_t bscan = 0; bscan < slidesClassifierStates.size(); ++bscan)
	{
		bpt::ptree& bscanNode = PTreeHelper::getNodeWithId(bscansTree, "BScan", static_cast<int>(bscan));
		slidesClassifierStates[bscan].saveState(bscanNode);
// 		if(bscanNode.size() == 1) // only ID
// 			bscanNode.clear();	// TODO: richtig löschen
// 			bscansTree.erase(bscanNode);
	}
}

void ScanClassifier::newSeriesLoaded(const OctData::Series* series, boost::property_tree::ptree& ptree)
{
	BscanMarkerBase::newSeriesLoaded(series, ptree);
	slidesClassifierStates.resize(series->bscanCount(), DefinedClassifierMarker::getInstance().getBscanMarkers());
	resetStates();

	loadState(ptree);
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

	scanClassifierProxys.resetChanges();
	slideClassifierProxys.resetChanges();
}

void ScanClassifier::updateStateProxys()
{
	setActBScan(getActBScanNr());
	scanClassifierProxys.setClassifierStates(&scanClassifierStates);
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

void ScanClassifier::ClassifierStates::loadState(const boost::property_tree::ptree& markerTree)
{
	for(ClassifierMarkerState& state: states)
		state.parsePTree(markerTree);
}

void ScanClassifier::ClassifierStates::saveState(boost::property_tree::ptree& markerTree) const
{
	for(const ClassifierMarkerState& state: states)
		state.fillPTree(markerTree);
}

bool ScanClassifier::ClassifierProxys::hasChanges() const
{
	for(const ClassifierMarkerProxy* proxy : proxys)
	{
		if(proxy && proxy->hasChanges())
			return true;
	}
	return false;
}

void ScanClassifier::ClassifierProxys::resetChanges()
{
	for(ClassifierMarkerProxy* proxy : proxys)
	{
		if(proxy)
			proxy->resetChanges();
	}
}
