#include "markermanager.h"
#include <octxmlread/octxml.h>
#include <octxmlread/markersxml.h>
#include <data_structure/cscan.h>
#include <data_structure/intervallmarker.h>
#include <data_structure/bscan.h>


#include <boost/exception/exception.hpp>
#include <boost/exception/diagnostic_information.hpp>

#include <iostream>
#include <QMessageBox>

MarkerManager::MarkerManager()
: QObject()
, cscan(new CScan)
{

}


MarkerManager::~MarkerManager()
{
	if(!xmlFilename.isEmpty() && dataChanged)
		saveMarkersXml(xmlFilename+"_markes.xml");
}



void MarkerManager::chooseBScan(int bscan)
{
	if(bscan == actBScan)
		return;
	if(bscan < 0)
		return;
	if(bscan >= static_cast<int>(cscan->bscanCount()))
		return;

	actBScan = bscan;

	emit(bscanChanged(actBScan));
}


void MarkerManager::loadImage(QString filename)
{
	if(!xmlFilename.isEmpty() && dataChanged)
		saveMarkersXml(xmlFilename+"_markes.xml");
	
	CScan* oldCScan = cscan;
	cscan = nullptr;

	try
	{
		cscan = new CScan;
		if(filename.toLower().endsWith(".xml"))
			OctXml::readOctXml(filename.toStdString(), cscan);
		
		xmlFilename = filename;
	}
	catch(...)
	{
		delete cscan;
		cscan = oldCScan;
		throw;
	}
	delete oldCScan;

	actBScan = 0;

	loadMarkersXml(xmlFilename+"_markes.xml");

	dataChanged = false;
	
	emit(newCScanLoaded());
}

void MarkerManager::initMarkerMap()
{
	std::size_t numBscans = cscan->bscanCount();
	markers.clear();
	markers.resize(numBscans);

	for(std::size_t i = 0; i<numBscans; ++i)
	{
		int bscanWidth = cscan->getBScan(i)->getWidth();
		markers[i].set(std::make_pair(boost::icl::discrete_interval<int>::closed(0, bscanWidth), IntervallMarker::Marker()));
	}
}


bool MarkerManager::cscanLoaded() const
{
	return cscan->bscanCount() > 0;
}

void MarkerManager::setMarker(int x1, int x2, const Marker& type, int bscan)
{
	if(!cscanLoaded())
		return;
	
	if(bscan<0 || bscan >= static_cast<int>(cscan->bscanCount()))
		return;

// 	printf("MarkerManager::setMarker(%d, %d, %d)", x1, x2, type);
// 	std::cout << std::endl;

	if(x2 < x1)
		std::swap(x1, x2);

	int maxWidth = cscan->getBScan(bscan)->getWidth();

	if(x2 < 0 || x1 > maxWidth)
		return;

	if(x1 < 0)
		x1 = 0;
	if(x2 >= maxWidth)
		x2 = maxWidth - 1;

	markers.at(bscan).set(std::make_pair(boost::icl::discrete_interval<int>::closed(x1, x2), type));
	dataChanged = true;
}

void MarkerManager::fillMarker(int x, const Marker& type)
{
	int bscan = actBScan;

	if(!cscanLoaded())
		return;

	if(bscan<0 || bscan >= static_cast<int>(cscan->bscanCount()))
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



void MarkerManager::loadMarkersXml(QString filename)
{
	initMarkerMap();
	addMarkersXml(filename);
}

void MarkerManager::addMarkersXml(QString filename)
{
	try
	{
		MarkersXML::readXML(this, filename.toStdString());
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
}


void MarkerManager::saveMarkersXml(QString filename)
{
	MarkersXML::writeXML(this, filename.toStdString());
}