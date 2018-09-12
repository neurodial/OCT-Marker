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

#ifndef PAINTMARKERMODEL_H
#define PAINTMARKERMODEL_H

#include <QModelIndex>


class BscanMarkerBase;

class PaintMarkerItem
{
	bool show = false;
	const BscanMarkerBase* const marker;
public:
	PaintMarkerItem(const BscanMarkerBase* marker)
	: marker(marker)
	{ }

	QString getName() const;

	bool isShowed() const { return show; }
	void setShow(bool b)  { show = b; }

	const BscanMarkerBase* getMarker() const { return marker; }
};


class PaintMarkerModel : public QAbstractTableModel
{
	Q_OBJECT

	std::vector<PaintMarkerItem> markers;

public:
	PaintMarkerModel();


	virtual QVariant data(const QModelIndex& index, int role) const override;
	bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole) override;
	Qt::ItemFlags flags(const QModelIndex& index) const override;

	virtual int columnCount(const QModelIndex& parent) const override;
	virtual int rowCount(const QModelIndex& parent) const override;
	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

	const std::vector<PaintMarkerItem> getMarkers()            const { return markers; }

public slots:
	void slotClicked(QModelIndex index);

signals:
	void viewChanged();
};

#endif // PAINTMARKERMODEL_H
