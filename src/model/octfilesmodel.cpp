#include "octfilesmodel.h"

#include <manager/octdatamanager.h>


OctFilesModel::OctFilesModel()
{
}


OctFilesModel::~OctFilesModel()
{
	for(const OctFileUnloaded* file : filelist)
		delete file;
}


QVariant OctFilesModel::data(const QModelIndex& index, int role) const
{
	if(!index.isValid())
		return QVariant();

	if(static_cast<std::size_t>(index.row()) >= filelist.size())
		return QVariant();

	if(role == Qt::DisplayRole)
		return filelist.at(index.row())->getFilename();
	else
		return QVariant();
}


QVariant OctFilesModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if(role != Qt::DisplayRole)
		return QVariant();

	if(orientation == Qt::Horizontal)
		return QString("Column %1").arg(section);
	else
		return QString("Row %1").arg(section);
}


bool OctFilesModel::addFile(QString filename)
{
	for(const OctFileUnloaded* file : filelist)
	{
		if(file->sameFile(filename))
			return false;
	}

	int position = filelist.size();
	
	beginInsertRows(QModelIndex(), position, position);
	filelist.push_back(new OctFileUnloaded(filename));
	endInsertRows();
	
	OctDataManager::getInstance().openFile(filename);
	
	return true;
}

void OctFilesModel::slotClicked(QModelIndex index)
{
	if(!index.isValid())
		return;
	
	std::size_t row = static_cast<std::size_t>(index.row());
	if(row >= filelist.size())
		return;
	
	OctFileUnloaded* file = filelist.at(row);
	OctDataManager::getInstance().openFile(file->getFilename());
}

void OctFilesModel::slotDoubleClicked(QModelIndex index)
{
	if(!index.isValid())
		return;
	
	std::size_t row = static_cast<std::size_t>(index.row());
	if(row >= filelist.size())
		return;
	
	OctFileUnloaded* file = filelist.at(row);
	
	
	qDebug("file doubleclicked: %s", file->getFilename().toStdString().c_str());
}


