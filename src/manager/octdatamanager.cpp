#include "octdatamanager.h"

#include <iostream>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/exception/diagnostic_information.hpp>
#include <boost/filesystem.hpp>

#include<QString>
#include<QTime>
#include<QMessageBox>
#include<QApplication>
#include<QFileInfo>
#include<QDir>

#include <octdata/datastruct/series.h>
#include <octdata/datastruct/bscan.h>
#include <octdata/datastruct/oct.h>
#include <octdata/octfileread.h>
#include <octdata/filereadoptions.h>
#include <octdata/filewriteoptions.h>

#include <helper/ptreehelper.h>
#include <data_structure/programoptions.h>
#include <data_structure/slobscandistancemap.h>

#include "octmarkerio.h"
#include "octmarkermanager.h"

namespace bpt = boost::property_tree;
namespace bfs = boost::filesystem;



OctDataManager::OctDataManager()
: markerstree(new bpt::ptree)
, markerIO(new OctMarkerIO(markerstree))
{
	connect(this, &OctDataManager::seriesChanged, this, &OctDataManager::clearSeriesCache);
}



OctDataManager::~OctDataManager()
{
	delete octData;
	delete markerstree;
	delete markerIO;
	delete seriesSLODistanceMap;
}


void OctDataManager::saveMarkersDefault()
{
	if(ProgramOptions::autoSaveOctMarkers())
		triggerSaveMarkersDefault();
}

void OctDataManager::triggerSaveMarkersDefault()
{
	if(!actFilename.isEmpty())
	{
		saveMarkerState(actSeries);
		markerIO->saveDefaultMarker(actFilename.toStdString());
		OctMarkerManager::getInstance().resetChangedSinceLastSaveState();
	}
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
		QFileInfo octmarkerPath(QApplication::applicationFilePath());

		OctData::FileReadOptions octOptions;
		octOptions.e2eGray             = static_cast<OctData::FileReadOptions::E2eGrayTransform>(ProgramOptions::e2eGrayTransform());
		octOptions.registerBScanns     = ProgramOptions::registerBScans();
		octOptions.fillEmptyPixelWhite = ProgramOptions::fillEmptyPixelWhite();
		octOptions.holdRawData         = ProgramOptions::holdOCTRawData();
		octOptions.readBScans          = ProgramOptions::readBScans();
		octOptions.rotateSlo           = ProgramOptions::loadRotateSlo();
		octOptions.libPath             = octmarkerPath.dir().absolutePath().toStdString(); // QApplication::applicationFilePath().toStdString();

		*oct = OctData::OctFileRead::openFile(filename.toStdString(), octOptions, this);
	}
	catch(boost::exception& e)
	{
		error = QString::fromStdString(boost::diagnostic_information(e));
		loadSuccess = false;
		loadError   = true;
	}
	catch(std::exception& e)
	{
		error = QString::fromStdString(e.what());
		loadSuccess = false;
		loadError   = true;
	}
	catch(const char* str)
	{
		error = str;
		loadSuccess = false;
		loadError   = true;
	}
	catch(...)
	{
		error = QString("Unknow error in file %1 line %2").arg(__FILE__).arg(__LINE__);
		loadSuccess = false;
		loadError   = true;
	}
}

bool OctDataManager::checkAndAskSaveBeforContinue()
{
	if(OctMarkerManager::getInstance().hasChangedSinceLastSave()) // TODO: move to new class for handel gui
	{
		QMessageBox::StandardButton result = QMessageBox::warning(nullptr, tr("Unsaved changes"), tr("You have unsaved changes, what will you do?"), QMessageBox::Discard | QMessageBox::Cancel | QMessageBox::Save);
		switch(result)
		{
			case QMessageBox::Cancel:
				return false;
			case QMessageBox::Discard:
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
	try
	{
		saveMarkersDefault();

		octData4Loading = new OctData::OCT;

		loadThread = new OctDataManagerThread(*this, filename, octData4Loading);
		connect(loadThread, &OctDataManagerThread::stepCalulated, this, &OctDataManager::loadOctDataThreadProgress);
		connect(loadThread, &OctDataManagerThread::finished     , this, &OctDataManager::loadOctDataThreadFinish  );
		loadThread->start();
	}
	catch(...)
	{
		loadFileSignal(false);
		throw;
	}
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
			QString error;
			try
			{
				markerstree->clear();
				markerIO->loadDefaultMarker(loadThread->getFilename().toStdString());
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


			actFilename = loadThread->getFilename();

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

			emit(octFileChanged());
			emit(octFileChanged(octData   ));
			emit(patientChanged(actPatient));
			emit(studyChanged  (actStudy  ));
			emit(seriesChanged (actSeries ));
			OctMarkerManager::getInstance().resetChangedSinceLastSaveState();
		}
	}
	else
	{
		if(loadThread->hasLoadError())
		{
			const QString& loadError = loadThread->getError();
			QMessageBox msgBox;
			msgBox.setText(tr("error open file: %1").arg(loadThread->getFilename()) + '\n' + loadError);
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
	{
		OctData::FileWriteOptions fwo;
		fwo.octBinFlat = ProgramOptions::saveOctBinFlat();

		OctData::OctFileRead::writeFile(filename.toStdString(), *octData, fwo);
	}
}

const SloBScanDistanceMap* OctDataManager::getSeriesSLODistanceMap() const
{
	if(!seriesSLODistanceMap)
	{
		seriesSLODistanceMap = new SloBScanDistanceMap();
		seriesSLODistanceMap->createData(actSeries);
	}

	return seriesSLODistanceMap;
}

void OctDataManager::clearSeriesCache()
{
	delete seriesSLODistanceMap;
	seriesSLODistanceMap = nullptr;
}

void OctDataManager::abortLoadingOctFile()
{
	if(loadThread)
		loadThread->breakLoad();
}
