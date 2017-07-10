#include "octfilesmodel.h"

#include <manager/octdatamanager.h>

#include <QMessageBox>
#include <boost/exception/diagnostic_information.hpp>


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
		return filelist.at(static_cast<std::size_t>(index.row()))->getFilename();
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

bool OctFilesModel::loadFile(QString filename)
{
	loadedFilePos = addFile(filename);
	return openFile(filename);
}



std::size_t OctFilesModel::addFile(QString filename)
{
	std::size_t count = 0;
	for(const OctFileUnloaded* file : filelist)
	{
		if(file->sameFile(filename))
		{
			return count;
		}
		++count;
	}

	beginInsertRows(QModelIndex(), static_cast<int>(loadedFilePos), static_cast<int>(loadedFilePos));
	filelist.push_back(new OctFileUnloaded(filename));
	endInsertRows();
	
	return count;
}

void OctFilesModel::loadNextFile()
{
	std::size_t filesInList = filelist.size();
	std::size_t requestFilePost = loadedFilePos + 1;
	if(requestFilePost < filesInList)
	{
		openFile(filelist[requestFilePost]->getFilename());
		loadedFilePos = requestFilePost;
		fileIdLoaded(index(loadedFilePos));
	}
}


void OctFilesModel::loadPreviousFile()
{
	if(loadedFilePos > 0)
	{
		--loadedFilePos;
		openFile(filelist[loadedFilePos]->getFilename());
		fileIdLoaded(index(loadedFilePos));
	}
}





void OctFilesModel::slotClicked(QModelIndex index)
{
	if(!index.isValid())
		return;
	
	std::size_t row = static_cast<std::size_t>(index.row());
	if(row >= filelist.size())
		return;
	

	loadedFilePos = row;
	OctFileUnloaded* file = filelist.at(row);
	openFile(file->getFilename());
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


bool OctFilesModel::openFile(const QString& filename)
{
	try
	{
		OctDataManager::getInstance().openFile(filename);
		return true;
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
	catch(const char* str)
	{
		QMessageBox msgBox;
		msgBox.setText(str);
		msgBox.setIcon(QMessageBox::Critical);
		msgBox.exec();
	}
	catch(const QString& str)
	{
		QMessageBox msgBox;
		msgBox.setText(str);
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
	return false;
}


