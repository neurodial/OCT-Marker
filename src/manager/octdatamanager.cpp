#include "octdatamanager.h"
#include <data_structure/programoptions.h>

#include <QString>
#include <QTime>



#include <octdata/datastruct/series.h>
#include <octdata/datastruct/bscan.h>
#include <octdata/datastruct/oct.h>
#include <octdata/octfileread.h>
#include <octdata/filereadoptions.h>



OctDataManager::OctDataManager()
{
}



OctDataManager::~OctDataManager()
{
	delete octData;
}



void OctDataManager::openFile(const QString& filename)
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

		OctData::Patient* patient = nullptr;
		OctData::Study* study     = nullptr;
		OctData::Series* series   = nullptr;
		
		if(octData->size() == 0)
			throw "MarkerManager::loadImage: oct->size() == 0";
		patient = octData->begin()->second;
		
		if(patient->size() > 0)
		{
			study = patient->begin()->second;
			
			if(study->size() > 0)
			{
				series = study->begin()->second;
			}
		}

		
		actFilename = filename;
		
		actPatient = patient;
		actStudy   = study;
		actSeries  = series;
	}
	catch(...)
	{
		delete octData;
		octData = oldOct;
		throw;
	}
	delete oldOct;

	emit(octFileChanged(octData   ));
	emit(patientChanged(actPatient));
	emit(studyChanged  (actStudy  ));
	emit(seriesChanged (actSeries ));
}


void OctDataManager::chooseSeries(const OctData::Series* seriesReq)
{
	qDebug("Series requested");
	
	if(seriesReq == actSeries)
		return;
	
	if(seriesReq == nullptr)
		return;
	
	// Search series
	for(const OctData::OCT::SubstructurePair& patientPair : *octData)
	{
		const OctData::Patient* patient = patientPair.second;
		for(const OctData::Patient::SubstructurePair& studyPair : *patient)
		{
			const OctData::Study* study = studyPair.second;
			for(const OctData::Study::SubstructurePair& seriesPair : *study)
			{
				const OctData::Series* series = seriesPair.second;
				if(series == seriesReq)
				{
					if(patient != actPatient)
					{
						actPatient = patient;
						emit(patientChanged(patient));
					}
					if(study != actStudy)
					{
						actStudy = study;
						emit(studyChanged(study));
					}
					if(series != actSeries)
					{
						actSeries = series;
						emit(seriesChanged(series));
					}
					qDebug("Series choosed");
					return;
				}
			}
		}
	}
}

bool OctDataManager::addMarkers(QString filename, OctDataManager::Fileformat format)
{
}

bool OctDataManager::loadMarkers(QString filename, OctDataManager::Fileformat format)
{
}

void OctDataManager::saveMarkers(QString filename, OctDataManager::Fileformat format)
{
}


