#ifndef OCTDATAMODEL_H
#define OCTDATAMODEL_H

#include <QModelIndex>
#include <QVariant>


class OctDataItemBase;

class OctDataModel : public QAbstractItemModel
{
	Q_OBJECT


	OctDataItemBase* rootElement = nullptr;

public:
	OctDataModel();
	virtual ~OctDataModel();

	QVariant data(const QModelIndex &index, int role) const;
	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

	// bool setData(const QModelIndex &index, const QVariant &value, int role);

	QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
	QModelIndex parent(const QModelIndex &index) const;

	int rowCount(const QModelIndex &parent = QModelIndex()) const;
	int columnCount(const QModelIndex &parent = QModelIndex()) const;


	// void setOctData(E2E::E2EData* data);
};

#endif // OCTDATAMODEL_H

