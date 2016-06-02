#include "markermanager.h"
#include <octxmlread/octxml.h>
#include <data_structure/cscan.h>


MarkerManager::MarkerManager()
: QObject()
, cscan(new CScan)
{

}


MarkerManager::~MarkerManager()
{

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

	emit(newCScanLoaded());
}
