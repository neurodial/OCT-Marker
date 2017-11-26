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
};


class PaintMarkerModel : public QAbstractTableModel
{
	std::vector<PaintMarkerItem> markers;

public:
	PaintMarkerModel();


	virtual QVariant data(const QModelIndex& index, int role) const override;
	virtual int columnCount(const QModelIndex& parent) const override;
	virtual int rowCount(const QModelIndex& parent) const override;
	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

public slots:
	void slotClicked(QModelIndex index);
};

#endif // PAINTMARKERMODEL_H
