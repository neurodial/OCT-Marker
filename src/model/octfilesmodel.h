#pragma once


#include <QVariant>
#include <QModelIndex>

#include <QString>
#include <vector>


class OctFileUnloaded
{
	QString filename;
public:
	OctFileUnloaded(const QString& filename) : filename(filename)   { }
	
	const QString& getFilename()       const                        { return filename; }
	bool sameFile(const QString& file) const                        { return filename == file; }
};


class OctFilesModel : public QAbstractListModel
{
	Q_OBJECT

	std::vector<OctFileUnloaded*> filelist;

	OctFilesModel();
	virtual ~OctFilesModel();

public:
	static OctFilesModel& getInstance()                             { static OctFilesModel instance; return instance;}
	
	
	int rowCount(const QModelIndex& /*parent*/ = QModelIndex()) const
	                                                                { return filelist.size(); }
	QVariant data(const QModelIndex &index, int role) const;
	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
	

private slots:
	
public slots:
	bool addFile(QString filename);
	
    void slotClicked(QModelIndex index);
    void slotDoubleClicked(QModelIndex index);
};

