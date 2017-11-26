#include "paintmarkermodel.h"


#include <manager/octmarkermanager.h>

#include <markermodules/bscanmarkerbase.h>



QString PaintMarkerItem::getName() const
{
	return marker->getName();
}




PaintMarkerModel::PaintMarkerModel()
{
	OctMarkerManager& manager = OctMarkerManager::getInstance();


	const std::vector<BscanMarkerBase*>& octMarkers = manager.getBscanMarker();

	for(const BscanMarkerBase* marker : octMarkers)
		markers.emplace_back(marker);
}


QVariant PaintMarkerModel::data(const QModelIndex& index, int role) const
{
	if(!index.isValid())
		return QVariant();

	if(role == Qt::CheckStateRole)
	{
		if(index.column() == 1)
		{
			const PaintMarkerItem& item = markers.at(index.row());
			return item.isShowed() ? Qt::Checked : Qt::Unchecked;
		}
	}


	if(role == Qt::DisplayRole)
	{
		const PaintMarkerItem& item = markers.at(index.row());

		switch(index.column())
		{
			case 0:
				return item.getName();
		}
	}

	return QVariant();
}

bool PaintMarkerModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
	if(role == Qt::CheckStateRole)
	{
		if(index.column() == 1)
		{
			PaintMarkerItem& item = markers.at(index.row());
			item.setShow(value.toBool());
			return true;
		}
	}
	return false;
}

Qt::ItemFlags PaintMarkerModel::flags(const QModelIndex& index) const
{
	if(!index.isValid())
		return 0;

	if(index.column() == 1)
		return Qt::ItemIsEnabled | Qt::ItemIsUserCheckable;

	return QAbstractTableModel::flags(index);
}


int PaintMarkerModel::columnCount(const QModelIndex& parent) const
{
	return 2;
}

int PaintMarkerModel::rowCount(const QModelIndex& parent) const
{
	return markers.size();
}


QVariant PaintMarkerModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if(role != Qt::DisplayRole)
		return QVariant();

	if(orientation == Qt::Horizontal)
	{
		switch(section)
		{
			case 0:
				return tr("marker name");
			case 1:
				return tr("show");
		}
	}
	else
		return QString("%1").arg(section);

	return QVariant();
}

void PaintMarkerModel::slotClicked(QModelIndex index)
{

	OctMarkerManager& manager = OctMarkerManager::getInstance();

	manager.setBscanMarker(index.row());
}
