#include "wgoctdatatree.h"

#include <model/octdatamodel.h>
#include <model/octdataitembase.h>
#include <QVBoxLayout>
#include <QTreeView>
#include <QHeaderView>


WGOctDataTree::WGOctDataTree(OctDataModel* octDataModel)
: viewTree(new QTreeView(this))
, model(octDataModel)
{
	viewTree->setModel(model);
	viewTree->setColumnWidth(0, 200);
	viewTree->header()->close() ;

	QVBoxLayout* layout = new QVBoxLayout(this);
	// layout->addWidget(label);
	layout->addWidget(viewTree);


	connect(viewTree, &QTreeView::clicked      , this, &WGOctDataTree::slotClicked      );
	connect(viewTree, &QTreeView::doubleClicked, this, &WGOctDataTree::slotDoubleClicked);
	connect(viewTree, &QTreeView::activated    , this, &WGOctDataTree::slotClicked      );
//	connect(viewTree, SIGNAL((QModelIndex)), this, SLOT(slotCliced(QModelIndex)));

	//viewTree->header()->setVisible(true);


// 	layout->setContentsMargins(0, 0, 0, 0);
	// viewTree->setFrameStyle(QFrame::NoFrame);
}


void WGOctDataTree::slotClicked(QModelIndex index)
{
	if(index.isValid())
	{
		OctDataItemBase* item = static_cast<OctDataItemBase*>(index.internalPointer());
		if(item)
			item->itemClicked();
	}
}
void WGOctDataTree::slotDoubleClicked(QModelIndex index)
{
	if(index.isValid())
	{
		OctDataItemBase* item = static_cast<OctDataItemBase*>(index.internalPointer());
		if(item)
			item->itemDoubleClicked();
	}
}
