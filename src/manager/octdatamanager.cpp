#include "octdatamanager.h"
#include <data_structure/programoptions.h>

#include <iostream>

#include <QString>
#include <QTime>

#include "octmarkerio.h"
#include "octmarkermanager.h"


#include <octdata/datastruct/series.h>
#include <octdata/datastruct/bscan.h>
#include <octdata/datastruct/oct.h>
#include <octdata/octfileread.h>
#include <octdata/filereadoptions.h>

#include <helper/ptreehelper.h>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/property_tree/json_parser.hpp>

#include <boost/lexical_cast.hpp>

#include <boost/exception/diagnostic_information.hpp>
#include <boost/filesystem.hpp>
#include <QMessageBox>

namespace bpt = boost::property_tree;
namespace bfs = boost::filesystem;



OctDataManager::OctDataManager()
: markerstree(new bpt::ptree)
, markerIO(new OctMarkerIO(markerstree))
{
}



OctDataManager::~OctDataManager()
{
	delete octData;
	delete markerstree;
	delete markerIO;
}


void OctDataManager::saveMarkersDefault()
{
	if(ProgramOptions::autoSaveOctMarkers())
		triggerSaveMarkersDefault();
}

void OctDataManager::triggerSaveMarkersDefault()
{
	saveMarkerState(actSeries);
	markerIO->saveDefaultMarker(actFilename.toStdString());
	OctMarkerManager::getInstance().resetChangedSinceLastSaveState();

}


void OctDataManagerThread::run()
{
	if(!oct)
	{
		error = "oct variable is not set";
		loadSuccess = false;
	}

	try
	{
		OctData::FileReadOptions octOptions;
		octOptions.e2eGray             = static_cast<OctData::FileReadOptions::E2eGrayTransform>(ProgramOptions::e2eGrayTransform());
		octOptions.registerBScanns     = ProgramOptions::registerBScans();
		octOptions.fillEmptyPixelWhite = ProgramOptions::fillEmptyPixelWhite();
		octOptions.holdRawData         = ProgramOptions::holdOCTRawData();
		octOptions.readBScans          = ProgramOptions::readBScans();
		octOptions.rotateSlo           = ProgramOptions::loadRotateSlo();

		*oct = OctData::OctFileRead::openFile(filename.toStdString(), octOptions, this);
	}
	catch(boost::exception& e)
	{
		error = QString::fromStdString(boost::diagnostic_information(e));
		loadSuccess = false;
	}
	catch(std::exception& e)
	{
		error = QString::fromStdString(e.what());
		loadSuccess = false;
	}
	catch(const char* str)
	{
		error = str;
		loadSuccess = false;
	}
	catch(...)
	{
		error = QString("Unknow error in file %1 line %2").arg(__FILE__).arg(__LINE__);
		loadSuccess = false;
	}
}

bool OctDataManager::checkAndAskSaveBeforContinue()
{
	if(OctMarkerManager::getInstance().hasChangedSinceLastSave()) // TODO: move to new class for handel gui
	{
		QMessageBox::StandardButton result = QMessageBox::warning(nullptr, tr("Unsaved changes"), tr("You have unsaved changes, what will you do?"), QMessageBox::Save|QMessageBox::Cancel|QMessageBox::Ignore);
		switch(result)
		{
			case QMessageBox::Cancel:
				return false;
			case QMessageBox::Ignore:
				break;
			case QMessageBox::Save:
				triggerSaveMarkersDefault();
				break;
			default:
				qDebug("OctDataManager::openFile: unexpected StandardButton %d", result);
				return false;;
		}
	}
	return true;
}


void OctDataManager::openFile(const QString& filename)
{
	if(loadThread)
		return;

	if(!ProgramOptions::autoSaveOctMarkers())
	{
		if(!checkAndAskSaveBeforContinue())
			return;
	}

	loadFileSignal(true);
	saveMarkersDefault();

	octData4Loading = new OctData::OCT;

	loadThread = new OctDataManagerThread(*this, filename, octData4Loading);
	connect(loadThread, &OctDataManagerThread::stepCalulated, this, &OctDataManager::loadOctDataThreadProgress);
	connect(loadThread, &OctDataManagerThread::finished     , this, &OctDataManager::loadOctDataThreadFinish  );
	loadThread->start();
}


void OctDataManager::loadOctDataThreadFinish()
{
	loadFileSignal(false);

	if(loadThread->success())
	{
		if(octData4Loading->size() == 0)
		{
			QMessageBox msgBox;
			msgBox.setText("OctDataManager::openFile: oct->size() == 0");
			msgBox.setIcon(QMessageBox::Critical);
			msgBox.exec();
		}
		else
		{
			delete octData;
			octData = octData4Loading;
			octData4Loading = nullptr;

			actPatient = octData->begin()->second;
			if(actPatient->size() > 0)
			{
				actStudy = actPatient->begin()->second;

				if(actStudy->size() > 0)
				{
					actSeries = actStudy->begin()->second;
				}
			}

			actFilename = loadThread->getFilename();

			QString error;
			try
			{
				markerstree->clear();
				markerIO->loadDefaultMarker(actFilename.toStdString());
			}
			catch(boost::exception& e)
			{
				error = QString::fromStdString(boost::diagnostic_information(e));
			}
			catch(std::exception& e)
			{
				error = QString::fromStdString(e.what());
			}
			catch(const char* str)
			{
				error = str;
			}
			catch(...)
			{
				error = QString("Unknow error in file %1 line %2").arg(__FILE__).arg(__LINE__);
			}
			if(!error.isEmpty())
			{
				QMessageBox msgBox;
				msgBox.setText("OctDataManager::openFile: markerload failed: " + error);
				msgBox.setIcon(QMessageBox::Critical);
				msgBox.exec();
			}


			emit(octFileChanged(octData   ));
			emit(patientChanged(actPatient));
			emit(studyChanged  (actStudy  ));
			emit(seriesChanged (actSeries ));

			loadFileSignal(false);

			OctMarkerManager::getInstance().resetChangedSinceLastSaveState();
		}
	}
	else
	{
		const QString& loadError = loadThread->getError();

		if(!loadError.isEmpty())
		{
			QMessageBox msgBox;
			msgBox.setText(loadError);
			msgBox.setIcon(QMessageBox::Critical);
			msgBox.exec();

		}
	}

	delete loadThread;
	delete octData4Loading;
	loadThread      = nullptr;
	octData4Loading = nullptr;
}



void OctDataManager::chooseSeries(const OctData::Series* seriesReq)
{
	saveMarkerState(actSeries);
	
	
	if(seriesReq == actSeries)
		return;
	
	const OctData::Patient* patient;
	const OctData::Study  * study;
	octData->findSeries(seriesReq, patient, study);

	if(!patient || !study)
		return;

	actPatient = patient;
	emit(patientChanged(patient));

	actStudy = study;
	emit(studyChanged(study));

	actSeries = seriesReq;
	emit(seriesChanged(seriesReq));
}


boost::property_tree::ptree* OctDataManager::getMarkerTreeSeries(const OctData::Series* seriesReq)
{
	if(!seriesReq)
		return nullptr;
	const OctData::Patient* patient;
	const OctData::Study  * study;
	octData->findSeries(seriesReq, patient, study);

	return getMarkerTreeSeries(patient, study, seriesReq);
}

boost::property_tree::ptree* OctDataManager::getMarkerTreeSeries(const OctData::Patient* patient, const OctData::Study* study, const OctData::Series* series)
{
	if(!patient || !study || !series)
		return nullptr;

	bpt::ptree& patNode    = PTreeHelper::getNodeWithId(*markerstree, "Patient", patient->getInternalId());
	bpt::ptree& studyNode  = PTreeHelper::getNodeWithId(patNode     , "Study"  , study  ->getInternalId());
	bpt::ptree& seriesNode = PTreeHelper::getNodeWithId(studyNode   , "Series" , series ->getInternalId());


	PTreeHelper::putNotEmpty(patNode   , "PatientUID", patient->getPatientUID());
	PTreeHelper::putNotEmpty(studyNode , "StudyUID"  , study  ->getStudyUID  ());
	PTreeHelper::putNotEmpty(seriesNode, "SeriesUID" , series ->getSeriesUID ());


	return &seriesNode;
}


/*
bool OctDataManager::addMarkers(QString filename, OctDataManager::Fileformat format)
{
	return false;
}
*/

bool OctDataManager::loadMarkers(QString filename, OctMarkerFileformat format)
{
	markerstree->clear();
	markerIO->loadMarkers(filename.toStdString(), format);
	emit(loadMarkerStateAll());
	OctMarkerManager::getInstance().resetChangedSinceLastSaveState();
	return true;
}

void OctDataManager::saveMarkers(QString filename, OctMarkerFileformat format)
{
	saveMarkerState(actSeries);
	markerIO->saveMarkers(filename.toStdString(), format);
	OctMarkerManager::getInstance().resetChangedSinceLastSaveState();
}

void OctDataManager::saveOctScan(QString filename)
{
	if(octData)
		OctData::OctFileRead::writeFile(filename.toStdString(), *octData);
}

