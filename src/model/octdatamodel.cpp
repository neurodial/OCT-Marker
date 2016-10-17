#include "octdatamodel.h"

#include <manager/markerdatamanager.h>


#include <octdata/datastruct/series.h>
#include <octdata/datastruct/study.h>
#include <octdata/datastruct/patient.h>
#include <octdata/datastruct/oct.h>

OctDataModel::OctDataModel()
{
	connect(&MarkerDataManager::getInstance(), &MarkerDataManager::octFileChanged, this, &OctDataModel::setOctData);
}


OctDataModel::~OctDataModel()
{
	clearList();
}

void OctDataModel::clearList()
{
	for(const OctSeriesItem* item : octSeriesList)
		delete item;
	octSeriesList.clear();
}


QVariant OctDataModel::data(const QModelIndex& index, int role) const
{
	if(!index.isValid())
		return QVariant();

	if(static_cast<std::size_t>(index.row()) >= octSeriesList.size())
		return QVariant();

	if(role == Qt::DisplayRole)
	{
		OctSeriesItem* item = octSeriesList.at(index.row());
		
		switch(index.column())
		{
			case 0:
				return item->getSurename();
			case 1:
				return item->getForename();
			case 2:
				return item->getStudyId();
			case 3:
				return item->getSeriesId();
		}
	}
	
	return QVariant();
}


QVariant OctDataModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if(role != Qt::DisplayRole)
		return QVariant();

	if(orientation == Qt::Horizontal)
	{
		switch(section)
		{
			case 0:
				return tr("Surname");
			case 1:
				return tr("Forename");
			case 2:
				return tr("Study");
			case 3:
				return tr("Series");
		}
	}
	else
		return QString("%1").arg(section);
	
	return QVariant();
}



void OctDataModel::slotClicked(QModelIndex index)
{
	if(!index.isValid())
		return;
	
	std::size_t row = static_cast<std::size_t>(index.row());
	if(row >= octSeriesList.size())
		return;
	
	OctSeriesItem* octItem = octSeriesList.at(row);
	MarkerDataManager::getInstance().chooseSeries(octItem->getSeries());
}

void OctDataModel::slotDoubleClicked(QModelIndex index)
{
	if(!index.isValid())
		return;
	
	std::size_t row = static_cast<std::size_t>(index.row());
	if(row >= octSeriesList.size())
		return;
	
	OctSeriesItem* file = octSeriesList.at(row);
	
	
	qDebug("file doubleclicked: %s", file->getForename().toStdString().c_str());
}


void OctDataModel::setOctData(const OctData::OCT* octData)
{
	beginResetModel();
	
	clearList();

	for(const OctData::OCT::SubstructurePair& patientPair : *octData)
	{
		const OctData::Patient* patient = patientPair.second;
		QString patName = QString("%1, %2").arg(patient->getSurname().c_str()).arg(patient->getForename().c_str());
		
		QString surname  = QString::fromStdString(patient->getSurname());
		QString forename = QString::fromStdString(patient->getForename());
		
		for(const OctData::Patient::SubstructurePair& studyPair : *patient)
		{
			const OctData::Study* study = studyPair.second;
			int studyID                 = studyPair.first;
			for(const OctData::Study::SubstructurePair& seriesPair : *study)
			{
				const OctData::Series* series = seriesPair.second;
				int seriesID = seriesPair.first;
				
				QString description = QString("Pat: %1, Study: %2, Series: %3").arg(patName).arg(studyID).arg(seriesID);
				
				OctSeriesItem* item = new OctSeriesItem(surname, forename, studyID, seriesID, series);
				
				octSeriesList.push_back(item);
				
			}
		}
	}
	
	endResetModel();
}

