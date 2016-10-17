#include "markerdatamanager.h"
#include <data_structure/programoptions.h>

#include <QString>
#include <QTime>



#include <octdata/datastruct/series.h>
#include <octdata/datastruct/bscan.h>
#include <octdata/datastruct/oct.h>
#include <octdata/octfileread.h>
#include <octdata/filereadoptions.h>


MarkerDataManager::~MarkerDataManager()
{
	delete octData;
}



void MarkerDataManager::openFile(const QString& filename)
{
	OctData::OCT* oldOct = octData;
	octData    = nullptr;

	try
	{
		QTime t;
		t.start();

		OctData::FileReadOptions octOptions;
		octOptions.e2eGray             = static_cast<OctData::FileReadOptions::E2eGrayTransform>(ProgramOptions::e2eGrayTransform());
		octOptions.registerBScanns     = ProgramOptions::registerBScans();
		octOptions.fillEmptyPixelWhite = ProgramOptions::fillEmptyPixelWhite();

		qDebug("Lese: %s", filename.toStdString().c_str());
		octData = new OctData::OCT(OctData::OctFileRead::openFile(filename.toStdString(), octOptions));
		qDebug("Dauer: %d ms", t.elapsed());

		// TODO: bessere Datenverwertung / Fehlerbehandlung
		if(octData->size() == 0)
			throw "MarkerManager::loadImage: oct->size() == 0";
		OctData::Patient* patient = octData->begin()->second;
		if(patient->size() == 0)
			throw "MarkerManager::loadImage: patient->size() == 0";
		OctData::Study* study = patient->begin()->second;
		if(study->size() == 0)
			throw "MarkerManager::loadImage: study->size() == 0";
		// series = study->begin()->second;

		actFilename = filename;
	}
	catch(...)
	{
		delete octData;
		octData = oldOct;
		throw;
	}
	delete oldOct;


	emit(octFileChanged(octData));
}


void MarkerDataManager::chooseSeries(OctData::Series*)
{
}

