#include "octdatamanager.h"
#include <data_structure/programoptions.h>

#include <QString>
#include <QTime>



#include <octdata/datastruct/series.h>
#include <octdata/datastruct/bscan.h>
#include <octdata/datastruct/oct.h>
#include <octdata/octfileread.h>
#include <octdata/filereadoptions.h>



#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/property_tree/json_parser.hpp>

#include <boost/lexical_cast.hpp>

namespace bpt = boost::property_tree;
namespace bfs = boost::filesystem;



OctDataManager::OctDataManager()
: markerstree(new bpt::ptree)
{
}



OctDataManager::~OctDataManager()
{
	delete octData;
	delete markerstree;
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
	markerstree->clear();

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

namespace
{
	template<typename T>
	bpt::ptree& get_put(bpt::ptree& tree, T path)
	{
		boost::optional<bpt::ptree&> child = tree.get_child_optional(path);
		if(child)
			return *child;
		return tree.add_child(path);
	}

	bpt::ptree& getNodeWithId(bpt::ptree& tree, const std::string& searchNode, int id)
	{
		for(std::pair<const std::string, bpt::ptree>& treePair : tree)
		{
			if(treePair.first != searchNode)
				continue;

			boost::optional<bpt::ptree&> child = treePair.second.get_child_optional("ID");
			if(child)
			{
				if(id == (*child).get_value<int>(id+1))
					return treePair.second;
			}
		}

		bpt::ptree& node = tree.add(searchNode, "");
		node.add("ID", id);

		return node;
	}

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

	bpt::ptree& patNode    = getNodeWithId(*markerstree, "Patient", patient->getInternalId());
	bpt::ptree& studyNode  = getNodeWithId(patNode     , "Study"  , study  ->getInternalId());
	bpt::ptree& seriesNode = getNodeWithId(studyNode   , "Series" , series ->getInternalId());

	return &seriesNode;
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


