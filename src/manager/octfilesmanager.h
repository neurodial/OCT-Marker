#ifndef OCTFILESMANAGER_H
#define OCTFILESMANAGER_H

#include <model/octdataitembase.h>


class OctFileUnloaded : public OctDataItemBase
{
	QString filename;
public:
    OctFileUnloaded(const QString& filename) : filename(filename) { name = filename; }
	const QString& getFilename()                                 { return filename; }
};

class OctFilesManager : public OctDataItemBase
{
	std::vector<OctFileUnloaded*> filelist;
	OctFilesManager();

	OctFilesManager(const OctFilesManager&)            = delete;
	OctFilesManager& operator=(const OctFilesManager&) = delete;

public:
	static OctFilesManager& getInstance()                        { static OctFilesManager instance; return instance; }

	virtual int childCount() const                               { return filelist.size(); }

public slots:
	virtual void loadOctData(QString filename);
};

#endif // OCTFILESMANAGER_H
