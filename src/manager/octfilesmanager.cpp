#include "octfilesmanager.h"
#include <model/octdatamodel.h>


OctFilesManager::OctFilesManager()
: OctDataItemBase()
{

}

OctFilesManager::~OctFilesManager()
{

}




void OctFilesManager::loadOctData(QString filename)
{
	OctFileUnloaded* item = new OctFileUnloaded(filename);
	OctDataModel& model = OctDataModel::getInstance();
	model.insertItem(item, childCount(), this);
}


bool OctFilesManager::insertChild(OctDataItemBase* item, int position)
{
	if(position != childCount())
		return false;

	addBaseElement(item);
	qDebug("Child added");
	return true;
}


bool OctFileUnloaded::insertChild(OctDataItemBase* item, int position)
{
	if(position != childCount())
		return false;

	addBaseElement(item);
	qDebug("OctFileUnloaded: Child added");
	return true;
}
