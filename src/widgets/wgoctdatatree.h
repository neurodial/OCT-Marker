#ifndef WGOCTDATATREE_H
#define WGOCTDATATREE_H

#include <QWidget>

class QTreeView;
class OctDataModel;

class WGOctDataTree : public QWidget
{
	Q_OBJECT

	QTreeView* viewTree = nullptr;
	OctDataModel* model = nullptr;

public:
	WGOctDataTree();


private slots:
    void slotClicked(QModelIndex index);
    void slotDoubleClicked(QModelIndex index);

};

#endif // WGOCTDATATREE_H
