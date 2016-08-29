#include "markermanager.h"
#include <octxmlread/markersreadwrite.h>
#include <data_structure/intervallmarker.h>
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

MarkerManager::MarkerManager()
: QObject()
, series(new OctData::Series)
{

}


MarkerManager::~MarkerManager()
{
	saveMarkerDefault();

	delete oct;
}

void MarkerManager::saveMarkerDefault()
{
	if(!xmlFilename.isEmpty() && dataChanged)
		saveMarkers(xmlFilename+".joctmarkers", Fileformat::Josn);
}

void MarkerManager::loadMarkerDefault()
{
	if(!loadMarkers(xmlFilename+".joctmarkers", Fileformat::Josn))
		loadMarkers(xmlFilename+"_markes.xml", Fileformat::XML);
	dataChanged = false;
}




void MarkerManager::chooseBScan(int bscan)
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


void MarkerManager::loadImage(QString filename)
{
	saveMarkerDefault();

	OctData::Series* oldSeries = series;
	OctData::OCT*    oldOct    = oct;
	series = nullptr;
	oct    = nullptr;

	try
	{
		QTime t;
		t.start();

		OctData::FileReadOptions octOptions;
		octOptions.e2eGray             = static_cast<OctData::FileReadOptions::E2eGrayTransform>(ProgramOptions::e2eGrayTransform());
		octOptions.registerBScanns     = ProgramOptions::registerBScanns();
		octOptions.fillEmptyPixelWhite = ProgramOptions::fillEmptyPixelWhite();

		qDebug("Lese: %s", filename.toStdString().c_str());
		oct = new OctData::OCT(OctData::OctFileRead::openFile(filename.toStdString(), octOptions));
		qDebug("Dauer: %d ms", t.elapsed());

		// TODO: bessere Datenverwertung / Fehlerbehandlung
		if(oct->size() == 0)
			throw "MarkerManager::loadImage: oct->size() == 0";
		OctData::Patient* patient = oct->begin()->second;
		if(patient->size() == 0)
			throw "MarkerManager::loadImage: patient->size() == 0";
		OctData::Study* study = patient->begin()->second;
		if(study->size() == 0)
			throw "MarkerManager::loadImage: study->size() == 0";
		series = study->begin()->second;
		
		xmlFilename = filename;
	}
	catch(...)
	{
		delete oct;
		oct    = oldOct;
		series = oldSeries;
		throw;
	}
	delete oldOct;

	actBScan = 0;

	loadMarkerDefault();
	
	emit(newCScanLoaded());
}

void MarkerManager::initMarkerMap()
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


bool MarkerManager::cscanLoaded() const
{
	return series->bscanCount() > 0;
}

void MarkerManager::setMarker(int x1, int x2, const Marker& type, int bscan)
{
	if(!cscanLoaded())
		return;
	
	if(bscan<0 || bscan >= static_cast<int>(series->bscanCount()))
		return;

// 	printf("MarkerManager::setMarker(%d, %d, %d)", x1, x2, type);
// 	std::cout << std::endl;

	if(x2 < x1)
		std::swap(x1, x2);

	int maxWidth = series->getBScan(bscan)->getWidth();

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



bool MarkerManager::loadMarkers(QString filename, Fileformat format)
{
	initMarkerMap();
	return addMarkers(filename, format);
}

bool MarkerManager::addMarkers(QString filename, Fileformat format)
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


void MarkerManager::saveMarkers(QString filename, Fileformat format)
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