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

	bool openFile(const QString& filename);

	std::size_t loadedFilePos = 0;

public:
	static OctFilesModel& getInstance()                             { static OctFilesModel instance; return instance;}
	
	
	int rowCount(const QModelIndex& /*parent*/ = QModelIndex()) const override
	                                                                { return static_cast<int>(filelist.size()); }
	QVariant data(const QModelIndex &index, int role) const override;
	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
	

private slots:
	
public slots:
	std::size_t addFile(QString filename);
	bool loadFile(QString filename);
	
	void slotClicked(QModelIndex index);
	void slotDoubleClicked(QModelIndex index);

	void loadNextFile();
	void loadPreviousFile();

signals:
	void fileIdLoaded(QModelIndex index);
};

