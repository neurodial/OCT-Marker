/*
 * Copyright (c) 2018 Kay Gawlik <kaydev@amarunet.de> <kay.gawlik@beuth-hochschule.de> <kay.gawlik@charite.de>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

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
	
	
	int rowCount(const QModelIndex& /*parent*/ = QModelIndex()) const override
	                                                                { return static_cast<int>(octSeriesList.size()); }

	int columnCount(const QModelIndex& /*parent*/ = QModelIndex()) const override
	                                                                { return 4; }
	
	QVariant data(const QModelIndex &index, int role) const override;
	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
	

private slots:

public slots:
	void setOctData(const OctData::OCT* octData);
	
	void slotClicked(QModelIndex index);
	void slotDoubleClicked(QModelIndex index);
};

#endif // OCTDATAMODEL_H

