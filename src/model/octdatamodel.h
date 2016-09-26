#ifndef OCTDATAMODEL_H
#define OCTDATAMODEL_H

#include <QVariant>
#include <QModelIndex>


class OctDataItemBase;

class OctDataModel : public QAbstractItemModel
{
	Q_OBJECT

	OctDataItemBase* rootElement = nullptr;

	OctDataModel();
	virtual ~OctDataModel();

public:
	static OctDataModel& getInstance()                           { static OctDataModel instance; return instance;}

	QVariant data(const QModelIndex &index, int role) const;
	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

	// bool setData(const QModelIndex &index, const QVariant &value, int role);

	QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
	QModelIndex parent(const QModelIndex &index) const;
	QModelIndex getQModelIndex(OctDataItemBase* item) const;

	int rowCount(const QModelIndex &parent = QModelIndex()) const;
	int columnCount(const QModelIndex &parent = QModelIndex()) const;

	bool insertItem(OctDataItemBase* item, int position, const QModelIndex& parent);
	bool insertItem(OctDataItemBase* item, int position, OctDataItemBase* parentItem);

	// void setOctData(E2E::E2EData* data);
private slots:
};

#endif // OCTDATAMODEL_H

