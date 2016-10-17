#ifndef OCTDATAMODEL_H
#define OCTDATAMODEL_H

#include <QVariant>
#include <QModelIndex>

#include <QAbstractTableModel>

#include <QString>
#include <vector>


namespace OctData
{
	class OCT;
	class Series;
}


class OctSeriesItem
{
	const QString surename;
	const QString forename;
	const int studyId;
	const int seriesId;
	const OctData::Series* const serie;
public:
	OctSeriesItem(const QString& surename, const QString& forename, int studyId, int seriesId, const OctData::Series* const series)
	: surename(surename)
	, forename(forename)
	, studyId(studyId)
	, seriesId(seriesId)
	, serie(series)
	{ }
	
	const QString& getSurename()       const                        { return surename; }
	const QString& getForename()       const                        { return forename; }
	int getStudyId()                   const                        { return studyId;  }
	int getSeriesId()                  const                        { return seriesId; }
	const OctData::Series* getSeries() const                        { return serie;    }
};


class OctDataModel : public QAbstractTableModel
{
	Q_OBJECT

	std::vector<OctSeriesItem*> octSeriesList;

	OctDataModel();
	virtual ~OctDataModel();
	
	void clearList();

public:
	static OctDataModel& getInstance()                              { static OctDataModel instance; return instance;}
	
	
	int rowCount(const QModelIndex& /*parent*/ = QModelIndex()) const
	                                                                { return octSeriesList.size(); }

	int columnCount(const QModelIndex& /*parent*/ = QModelIndex()) const
	                                                                { return 4; }
	
	QVariant data(const QModelIndex &index, int role) const;
	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
	

private slots:
	
public slots:
	void setOctData(const OctData::OCT* octData);
	
    void slotClicked(QModelIndex index);
    void slotDoubleClicked(QModelIndex index);
};

#endif // OCTDATAMODEL_H

