#include "octdatamodel.h"
#include "octdataitembase.h"
#include <manager/octfilesmanager.h>


OctDataModel::OctDataModel()
: rootElement(&OctFilesManager::getInstance())
{

}


OctDataModel::~OctDataModel()
{
	// delete rootElement;
}



int OctDataModel::columnCount(const QModelIndex& /*parent*/) const
{
	return 1;
}

QVariant OctDataModel::data(const QModelIndex& index, int role) const
{
	if(!index.isValid())
		return QVariant();

	if(role != Qt::DisplayRole)
		return QVariant();


	OctDataItemBase* item = static_cast<OctDataItemBase*>(index.internalPointer());

	// return item->;
	switch(index.column())
	{
		case 0:
			return QVariant(item->getName());

	}
	return QVariant();
}

QVariant OctDataModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if(orientation == Qt::Horizontal && role == Qt::DisplayRole)
	{
		switch(section)
		{
			case 0:
				return tr("Name");
		}
	}

	return QVariant();
}



QModelIndex OctDataModel::index(int row, int column, const QModelIndex& parentIndex) const
{
	if(!hasIndex(row, column, parentIndex))
		return QModelIndex();


	OctDataItemBase* parentItem;

	if(!parentIndex.isValid())
		parentItem = rootElement;
	else
		parentItem = static_cast<OctDataItemBase*>(parentIndex.internalPointer());

	if(parentItem)
	{
		OctDataItemBase* childItem = parentItem->child(row);
		if(childItem)
			return createIndex(row, column, childItem);
	}

	return QModelIndex();
}

QModelIndex OctDataModel::parent(const QModelIndex& index) const
{
	if(!index.isValid())
		return QModelIndex();

	OctDataItemBase* childItem = static_cast<OctDataItemBase*>(index.internalPointer());
	OctDataItemBase* parentItem = childItem->parent();

	Q_ASSERT(parentItem != nullptr);

	if(parentItem == rootElement)
		return QModelIndex();


	return createIndex(parentItem->row(), 0, parentItem);
}

QModelIndex OctDataModel::getQModelIndex(OctDataItemBase* item) const
{
	if(!item || item == rootElement)
		return QModelIndex();

	return createIndex(item->row(), 0, item);
}


int OctDataModel::rowCount(const QModelIndex& parentIndex) const
{
	OctDataItemBase* parentItem;

	parentIndex.internalPointer();

	if(!parentIndex.isValid())
		parentItem = rootElement;
	else
		parentItem = static_cast<OctDataItemBase*>(parentIndex.internalPointer());

	if(parentItem)
		return parentItem->childCount();

	return 0;
}


bool OctDataModel::insertItem(OctDataItemBase* item, int position, OctDataItemBase* parentItem)
{
	return insertItem(item, position, getQModelIndex(parentItem));
}



bool OctDataModel::insertItem(OctDataItemBase* item, int position, const QModelIndex& parent)
{
	OctDataItemBase* parentItem = static_cast<OctDataItemBase*>(parent.internalPointer());
	if(!parentItem)
		parentItem = rootElement;

	if(position == -1)
		position = parentItem->childCount();
	else
	{
		if(parentItem->childCount() < position)
			return false;
	}

	beginInsertRows(parent, position, position);
	bool success = parentItem->insertChild(item, position);
	// success = parentItem->insertChildren(position, rows, rootItem->columnCount());
	endInsertRows();

/*
	beginResetModel();
	endResetModel();
*/

	return success;
}

