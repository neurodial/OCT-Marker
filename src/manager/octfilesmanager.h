#ifndef OCTFILESMANAGER_H
#define OCTFILESMANAGER_H

#include <model/octdataitembase.h>

class QModelIndex;

class OctFileUnloaded : public OctDataItemBase
{
	QString filename;
public:
	OctFileUnloaded(const QString& filename) : filename(filename) { name = filename; }
	const QString& getFilename()                                 { return filename; }
	virtual bool insertChild(OctDataItemBase* item, int position);

	bool sameFile(const QString& file)                           { return filename == file; }
};

class OctFilesManager : public OctDataItemBase
{
	OctFilesManager();

	OctFilesManager(const OctFilesManager&)            = delete;
	OctFilesManager& operator=(const OctFilesManager&) = delete;

	~OctFilesManager();
public:
	static OctFilesManager& getInstance()                        { static OctFilesManager instance; return instance; }


	virtual bool canRemoveChild() { return false; }
	virtual bool removeChild(OctDataItemBase* /*item*/)         { return false; }

	virtual bool insertChild(OctDataItemBase* item, int position);

public slots:
	virtual void loadOctData(QString filename);

};

#endif // OCTFILESMANAGER_H
