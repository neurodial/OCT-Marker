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

BScanMarkerManager::BScanMarkerManager()
: QObject()
{
	
	OctDataManager& dataManager = OctDataManager::getInstance();
	connect(&dataManager, &OctDataManager::seriesChanged , this, &BScanMarkerManager::showSeries);

	markerObj.push_back(new BScanSegmentation(this));
	markerObj.push_back(new BScanIntervalMarker(this));
	
	for(BscanMarkerBase* obj : markerObj)
		obj->activate(false);
	
	setMarker(0);
}


BScanMarkerManager::~BScanMarkerManager()
{
	for(BscanMarkerBase* obj : markerObj)
		delete obj;
//	saveMarkerDefault();
}

/*
void BScanMarkerManager::saveMarkerDefault()
{
	if(!xmlFilename.isEmpty() && dataChanged)
		saveMarkers(xmlFilename+".joctmarkers", Fileformat::Josn);
}

void BScanMarkerManager::loadMarkerDefault()
{
	if(!loadMarkers(xmlFilename+".joctmarkers", Fileformat::Josn))
		loadMarkers(xmlFilename+"_markes.xml", Fileformat::XML);
	dataChanged = false;
}
*/



void BScanMarkerManager::chooseBScan(int bscan)
{
	if(bscan == actBScan)
		return;
	if(bscan < 0)
		return;
	if(bscan >= static_cast<int>(series->bscanCount()))
		return;

	actBScan = bscan;

	emit(bscanChanged(actBScan));
}

/*
void BScanMarkerManager::initMarkerMap()
{
	std::size_t numBscans = series->bscanCount();
	markers.clear();
	markers.resize(numBscans);

	for(std::size_t i = 0; i<numBscans; ++i)
	{
		int bscanWidth = series->getBScan(i)->getWidth();
		markers[i].set(std::make_pair(boost::icl::discrete_interval<int>::closed(0, bscanWidth), IntervallMarker::Marker()));
	}
}


bool BScanMarkerManager::cscanLoaded() const
{
	if(!series)
		return false;
	return series->bscanCount() > 0;
}

void BScanMarkerManager::fillMarker(int x, const Marker& type)
{
	int bscan = actBScan;

	if(!cscanLoaded())
		return;

	if(bscan<0 || bscan >= static_cast<int>(series->bscanCount()))
		return;

	MarkerMap& map = markers.at(bscan);
	MarkerMap::const_iterator it = map.find(x);

	boost::icl::discrete_interval<int> intervall = it->first;

	if(intervall.upper() > 0)
	{
		markers.at(bscan).set(std::make_pair(boost::icl::discrete_interval<int>::closed(intervall.lower(), intervall.upper()), type));
		dataChanged = true;
	}

}



bool BScanMarkerManager::loadMarkers(QString filename, Fileformat format)
{
	initMarkerMap();
	return addMarkers(filename, format);
}

bool BScanMarkerManager::addMarkers(QString filename, Fileformat format)
{
	bool result = false;
	try
	{
		switch(format)
		{
			case Fileformat::XML:
				result = MarkersReadWrite::readXML(this, filename.toStdString());
				break;
			case Fileformat::Josn:
				result = MarkersReadWrite::readJosn(this, filename.toStdString());
				break;
		}
		dataChanged = true;
		emit(bscanChanged(actBScan)); // TODO: implementiere eigenes Signal
	}
	catch(boost::exception& e)
	{
		QMessageBox msgBox;
		msgBox.setText(QString::fromStdString(boost::diagnostic_information(e)));
		msgBox.setIcon(QMessageBox::Critical);
		msgBox.exec();
	}
	catch(std::exception& e)
	{
		QMessageBox msgBox;
		msgBox.setText(QString::fromStdString(e.what()));
		msgBox.setIcon(QMessageBox::Critical);
		msgBox.exec();
	}
	catch(...)
	{
		QMessageBox msgBox;
		msgBox.setText(QString("Unknow error in file %1 line %2").arg(__FILE__).arg(__LINE__));
		msgBox.setIcon(QMessageBox::Critical);
		msgBox.exec();
	}
	return result;
}


void BScanMarkerManager::saveMarkers(QString filename, Fileformat format)
{
	switch(format)
	{
		case Fileformat::XML:
			MarkersReadWrite::writeXML(this, filename.toStdString());
			break;
		case Fileformat::Josn:
			MarkersReadWrite::writeJosn(this, filename.toStdString());
			break;
	}
}
*/

void BScanMarkerManager::showSeries(const OctData::Series* s)
{
	series = s;
	
	actBScan = 0;

	// loadMarkerDefault();
	
	emit(newSeriesShowed(s));
}


void BScanMarkerManager::setMarker(int id)
{
	BscanMarkerBase* newMarker = nullptr;
		
	if(id < 0 || id >= markerObj.size())
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


