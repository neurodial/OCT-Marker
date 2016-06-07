#include "markermanager.h"
#include <octxmlread/octxml.h>
#include <octxmlread/markersxml.h>
#include <data_structure/cscan.h>
#include <data_structure/intervallmarker.h>
#include <data_structure/bscan.h>


#include <iostream>

MarkerManager::MarkerManager()
: QObject()
, cscan(new CScan)
{

}


MarkerManager::~MarkerManager()
{
	saveMarkersXml(xmlFilename+"_markes.xml");
}



void MarkerManager::chooseBScan(int bscan)
{
	if(bscan == actBScan)
		return;
	if(bscan < 0)
		return;
	if(bscan >= cscan->bscanCount())
		return;

	actBScan = bscan;

	emit(bscanChanged(actBScan));
}


void MarkerManager::loadOCTXml(QString filename)
{
	delete cscan;
	cscan = new CScan;

	OctXml::readOctXml(filename.toStdString(), cscan);

	actBScan = 0;

	// showBScan();

	std::size_t numBscans = cscan->bscanCount();
	markers.clear();
	markers.resize(numBscans);

	xmlFilename = filename;

	loadMarkersXml(xmlFilename+"_markes.xml");
	
	emit(newCScanLoaded());
}

bool MarkerManager::cscanLoaded() const
{
	return cscan->bscanCount() > 0;
}

void MarkerManager::setMarker(int x1, int x2, int type, int bscan)
{
	if(!cscanLoaded())
		return;
	
	if(type <= -2)
		type = markerId;
	
	if(bscan<0 || bscan >= cscan->bscanCount())
		return;

// 	printf("MarkerManager::setMarker(%d, %d, %d)", x1, x2, type);
// 	std::cout << std::endl;

	if(type > IntervallMarker::getInstance().size())
		return;

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
}


void MarkerManager::loadMarkersXml(QString filename)
{
	MarkersXML::readXML(this, filename.toStdString());
}

void MarkerManager::saveMarkersXml(QString filename)
{
	MarkersXML::writeXML(this, filename.toStdString());
}

