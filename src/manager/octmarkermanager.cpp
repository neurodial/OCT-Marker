#include "octmarkermanager.h"

#include <data_structure/intervalmarker.h>
#include <data_structure/programoptions.h>

#include <octdata/datastruct/series.h>
#include <octdata/datastruct/bscan.h>
#include <octdata/datastruct/oct.h>
#include <octdata/octfileread.h>
#include <octdata/filereadoptions.h>

#include <boost/exception/exception.hpp>
#include <boost/exception/diagnostic_information.hpp>

#include <iostream>
#include <QMessageBox>

#include <QTime>
#include "octdatamanager.h"

#include <markermodules/bscanmarkerbase.h>
#include <markermodules/slomarkerbase.h>
#include <markermodules/bscanintervalmarker/bscanintervalmarker.h>
#include <markermodules/bscansegmentation/bscansegmentation.h>
#include <markermodules/bscanlayersegmentation/bscanlayersegmentation.h>
#include <markermodules/sloobjects/sloobjectmarker.h>

#include <helper/ptreehelper.h>

#include <boost/property_tree/ptree.hpp>

#include <helper/signalblocker.h>

namespace bpt = boost::property_tree;


OctMarkerManager::OctMarkerManager()
: QObject()
{
	
	OctDataManager& dataManager = OctDataManager::getInstance();
	connect(&dataManager, &OctDataManager::seriesChanged     , this, &OctMarkerManager::showSeries         );
	connect(&dataManager, &OctDataManager::saveMarkerState   , this, &OctMarkerManager::saveMarkerStateSlot);
	connect(&dataManager, &OctDataManager::loadMarkerState   , this, &OctMarkerManager::loadMarkerStateSlot);
	connect(&dataManager, &OctDataManager::loadMarkerStateAll, this, &OctMarkerManager::reloadMarkerStateSlot);

	bscanMarkerObj.push_back(new BScanSegmentation(this));
	bscanMarkerObj.push_back(new BScanIntervalMarker(this));
	bscanMarkerObj.push_back(new BScanLayerSegmentation(this));
	
	for(BscanMarkerBase* obj : bscanMarkerObj)
	{
		obj->activate(false);
		connect(obj, &BscanMarkerBase::requestFullUpdate, this, &OctMarkerManager::udateFromMarkerModul            );
		connect(obj, &BscanMarkerBase::sloViewHasChanged, this, &OctMarkerManager::handleSloRedrawAfterMarkerChange);
	}


	sloMarkerObj.push_back(new SloObjectMarker(this));
	
	setBscanMarker(ProgramOptions::bscanMarkerToolId());
	setSloMarker  (ProgramOptions::  sloMarkerToolId());
}


OctMarkerManager::~OctMarkerManager()
{
	for(BscanMarkerBase* obj : bscanMarkerObj)
		delete obj;
}



void OctMarkerManager::chooseBScan(int bscan)
{
	if(bscan == actBScan)
		return;
	if(bscan < 0)
		return;
	if(bscan >= static_cast<int>(series->bscanCount()))
		return;

	actBScan = bscan;

	if(actBscanMarker)
		actBscanMarker->setActBScan(actBScan);

	emit(newBScanShowed(series->getBScan(actBScan)));
	emit(bscanChanged(actBScan));
}


void OctMarkerManager::showSeries(const OctData::Series* s)
{
	series = s;
	bpt::ptree* markerTree = OctDataManager::getInstance().getMarkerTree(s);

	if(!markerTree)
		return;

	actBScan = 0;

	if(actBscanMarker && !stateChangedSinceLastSave)
		stateChangedSinceLastSave = actBscanMarker->hasChangedSinceLastSave();

	if(actSloMarker && !stateChangedSinceLastSave)
		stateChangedSinceLastSave = actSloMarker->hasChangedSinceLastSave();


	for(BscanMarkerBase* obj : bscanMarkerObj)
	{
		SignalBlocker sigBlock(obj);
		const QString& markerId = obj->getMarkerId();
		bpt::ptree& subtree = PTreeHelper::get_put(*markerTree, markerId.toStdString());
		obj->newSeriesLoaded(s, subtree);
	}

	for(SloMarkerBase* obj : sloMarkerObj)
	{
		const QString& markerId = obj->getMarkerId();
		bpt::ptree& subtree = PTreeHelper::get_put(*markerTree, markerId.toStdString());
		obj->newSeriesLoaded(s, subtree);
	}

	emit(newBScanShowed(series->getBScan(actBScan)));
	emit(newSeriesShowed(s));
}


void OctMarkerManager::setBscanMarker(int id)
{
	BscanMarkerBase* newMarker = nullptr;
		
	if(id < 0 || static_cast<std::size_t>(id) >= bscanMarkerObj.size())
		newMarker = nullptr;
	else
		newMarker = bscanMarkerObj.at(id);
	
	if(newMarker != actBscanMarker)
	{
		actBscanMarkerId = id;
		if(actBscanMarker)
		{
			actBscanMarker->activate(false);
			stateChangedSinceLastSave &= actBscanMarker->hasChangedSinceLastSave();
		}
		if(newMarker)
		{
			newMarker->setActBScan(actBScan);
			newMarker->activate(true);
		}
		
		actBscanMarker = newMarker;
		emit(bscanChanged(actBScan));
		emit(bscanMarkerChanged(actBscanMarker));
		ProgramOptions::bscanMarkerToolId.setValue(actBscanMarkerId);
	}
}

void OctMarkerManager::setSloMarker(int id)
{
	SloMarkerBase* newMarker = nullptr;

	if(id < 0 || static_cast<std::size_t>(id) >= bscanMarkerObj.size())
		newMarker = nullptr;
	else
		newMarker = sloMarkerObj.at(id);

	if(newMarker != actSloMarker)
	{
		actSloMarkerId = id;
		if(actSloMarker)
			actSloMarker->activate(false);
		if(newMarker)
			newMarker->activate(true);

		actSloMarker = newMarker;
		emit(sloMarkerChanged(actSloMarker));
		ProgramOptions::sloMarkerToolId.setValue(actSloMarkerId);
	}
}



void OctMarkerManager::saveMarkerStateSlot(const OctData::Series* s)
{
	if(series != s)
		return;

	bpt::ptree* markerTree = OctDataManager::getInstance().getMarkerTree(s);

	if(!markerTree)
		return;


	for(BscanMarkerBase* obj : bscanMarkerObj)
	{
		const QString& markerId = obj->getMarkerId();
		bpt::ptree& subtree = PTreeHelper::get_put(*markerTree, markerId.toStdString());
		obj->saveState(subtree);
	}

	for(SloMarkerBase* obj : sloMarkerObj)
	{
		const QString& markerId = obj->getMarkerId();
		bpt::ptree& subtree = PTreeHelper::get_put(*markerTree, markerId.toStdString());
		obj->saveState(subtree);
	}
}


void OctMarkerManager::loadMarkerStateSlot(const OctData::Series* s)
{
	if(series != s)
		return;


	bpt::ptree* markerTree = OctDataManager::getInstance().getMarkerTree(s);

	if(!markerTree)
		return;


	for(BscanMarkerBase* obj : bscanMarkerObj)
	{
		SignalBlocker sigBlock(obj);

		const QString& markerId = obj->getMarkerId();
		bpt::ptree& subtree = PTreeHelper::get_put(*markerTree, markerId.toStdString());
		obj->loadState(subtree);
	}

	for(SloMarkerBase* obj : sloMarkerObj)
	{
		const QString& markerId = obj->getMarkerId();
		bpt::ptree& subtree = PTreeHelper::get_put(*markerTree, markerId.toStdString());
		obj->loadState(subtree);
	}
}


void OctMarkerManager::udateFromMarkerModul()
{
	 QObject* obj = sender();
	 if(obj == actBscanMarker)
	 {
		 emit(bscanChanged(actBScan));
	 }
}

bool OctMarkerManager::hasActMarkerChanged() const
{
	bool result = false;
	if(actBscanMarker) result |= actBscanMarker->hasChangedSinceLastSave();
	if(actSloMarker)   result |= actSloMarker  ->hasChangedSinceLastSave();
	return result;
}

void OctMarkerManager::handleSloRedrawAfterMarkerChange()
{
	emit(sloViewChanged());
}

