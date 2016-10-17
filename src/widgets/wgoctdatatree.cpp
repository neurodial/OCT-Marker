#include "wgoctdatatree.h"

#include <model/octdatamodel.h>
#include <model/octfilesmodel.h>
#include <QVBoxLayout>
#include <QListView>
#include <QHeaderView>


WGOctDataTree::WGOctDataTree()
: listviewFiles(new QListView(this))
, modelFiles(&OctFilesModel::getInstance())
, listviewOctData(new QListView(this))
, modelOctData(&OctDataModel::getInstance())
{
	listviewFiles->setModel(modelFiles);
	listviewOctData->setModel(modelOctData);
// 	listview->setColumnWidth(0, 200);
// 	listview->header()->close() ;

	QVBoxLayout* layout = new QVBoxLayout(this);
	// layout->addWidget(label);
	layout->addWidget(listviewFiles);
	layout->addWidget(listviewOctData);


	connect(listviewFiles, &QListView::clicked      , modelFiles, &OctFilesModel::slotClicked      );
	connect(listviewFiles, &QListView::doubleClicked, modelFiles, &OctFilesModel::slotDoubleClicked);
	connect(listviewFiles, &QListView::activated    , modelFiles, &OctFilesModel::slotClicked      );
//	connect(listview, SIGNAL((QModelIndex)), this, SLOT(slotCliced(QModelIndex)));

	//viewTree->header()->setVisible(true);


// 	layout->setContentsMargins(0, 0, 0, 0);
	// viewTree->setFrameStyle(QFrame::NoFrame);
}

