#include "octfilesmanager.h"


OctFilesManager::OctFilesManager()
: OctDataItemBase()
{
	filelist.push_back(new OctFileUnloaded("test 123"));
	filelist.push_back(new OctFileUnloaded("test 456"));

}



void OctFilesManager::loadOctData(QString filename)
{
	filelist.push_back(new OctFileUnloaded(filename));
}
