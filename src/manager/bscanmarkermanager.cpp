#include "bscanmarkermanager.h"

#include <octxmlread/markersreadwrite.h>
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
#include <markermodules/bscanintervalmarker/bscanintervalmarker.h>
#include <markermodules/bscansegmentation//bscansegmentation.h>

#include <helper/ptreehelper.h>

#include <boost/property_tree/ptree.hpp>
namespace bpt = boost::property_tree;


BScanMarkerManager::BScanMarkerManager()
: QObject()
{
	
	OctDataManager& dataManager = OctDataManager::getInstance();
	connect(&dataManager, &OctDataManager::seriesChanged  , this, &BScanMarkerManager::showSeries         );
	connect(&dataManager, &OctDataManager::saveMarkerState, this, &BScanMarkerManager::saveMarkerStateSlot);
	connect(&dataManager, &OctDataManager::loadMarkerState, this, &BScanMarkerManager::loadMarkerStateSlot);

	markerObj.push_back(new BScanSegmentation(this));
	markerObj.push_back(new BScanIntervalMarker(this));
	
	for(BscanMarkerBase* obj : markerObj)
	{
		obj->activate(false);
		connect(obj, &BscanMarkerBase::requestUpdate, this, &BScanMarkerManager::udateFromMarkerModul);
	}
	
	setMarker(0);
}


BScanMarkerManager::~BScanMarkerManager()
{
	for(BscanMarkerBase* obj : markerObj)
		delete obj;
//	saveMarkerDefault();
}



void BScanMarkerManager::chooseBScan(int bscan)
{
	if(bscan == actBScan)
		return;
	if(bscan < 0)
		return;
	if(bscan >= static_cast<int>(series->bscanCount()))
		return;

	actBScan = bscan;

	emit(newBScanShowed(series->getBScan(actBScan)));
	emit(bscanChanged(actBScan));
}


void BScanMarkerManager::showSeries(const OctData::Series* s)
{
	series = s;
	markerTree = OctDataManager::getInstance().getMarkerTree(s);
	
	actBScan = 0;


	for(BscanMarkerBase* obj : markerObj)
	{
		const QString& markerId = obj->getMarkerId();
		bpt::ptree& subtree = PTreeHelper::get_put(*markerTree, markerId.toStdString());
		obj->newSeriesLoaded(s, subtree);
	}


	emit(newBScanShowed(series->getBScan(actBScan)));
	emit(newSeriesShowed(s));
}


void BScanMarkerManager::setMarker(int id)
{
	BscanMarkerBase* newMarker = nullptr;
		
	if(id < 0 || static_cast<std::size_t>(id) >= markerObj.size())
		newMarker = nullptr;
	else
		newMarker = markerObj.at(id);
	
	if(newMarker != actMarker)
	{
		actMarkerId = id;
		if(actMarker)
			actMarker->activate(false);
		if(newMarker)
			newMarker->activate(true);
		
		actMarker = newMarker;
		emit(bscanChanged(actBScan));
	}
}


void BScanMarkerManager::saveMarkerStateSlot(const OctData::Series* s)
{
	if(series != s)
		return;

	if(!markerTree)
		return;


	for(BscanMarkerBase* obj : markerObj)
	{
		const QString& markerId = obj->getMarkerId();
		bpt::ptree& subtree = PTreeHelper::get_put(*markerTree, markerId.toStdString());
		obj->saveState(subtree);
	}
}


void BScanMarkerManager::loadMarkerStateSlot(const OctData::Series* s)
{
	if(series != s)
		return;

	markerTree = OctDataManager::getInstance().getMarkerTree(s);

	if(!markerTree)
		return;


	for(BscanMarkerBase* obj : markerObj)
	{
		const QString& markerId = obj->getMarkerId();
		bpt::ptree& subtree = PTreeHelper::get_put(*markerTree, markerId.toStdString());
		obj->loadState(subtree);
	}
}


void BScanMarkerManager::udateFromMarkerModul()
{
	 QObject* obj = sender();
	 if(obj == actMarker)
	 {
		 emit(bscanChanged(actBScan));
	 }
}

