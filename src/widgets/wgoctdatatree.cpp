#include "wgoctdatatree.h"

#include <model/octdatamodel.h>
#include <model/octfilesmodel.h>
#include <QSplitter>
#include <QVBoxLayout>
#include <QListView>
#include <QHeaderView>

#include <QTableView>


WGOctDataTree::WGOctDataTree()
: listviewFiles(new QListView(this))
, modelFiles(&OctFilesModel::getInstance())
, listviewOctData(new QTableView(this))
, modelOctData(&OctDataModel::getInstance())
{
	listviewFiles->setModel(modelFiles);
	listviewOctData->setModel(modelOctData);
// 	listview->setColumnWidth(0, 200);
// 	listview->header()->close() ;
	
	listviewOctData->setSelectionBehavior(QAbstractItemView::SelectRows);
	listviewOctData->setSelectionMode(QAbstractItemView::SingleSelection);

	QVBoxLayout* layout = new QVBoxLayout(this);
	
	QSplitter* splitter = new QSplitter(this);
	splitter->setOrientation(Qt::Vertical);
	
	splitter->addWidget(listviewFiles);
	splitter->addWidget(listviewOctData);

	layout->addWidget(splitter);

	connect(listviewFiles  , &QListView::clicked       , modelFiles  , &OctFilesModel::slotClicked      );
	connect(listviewFiles  , &QListView::doubleClicked , modelFiles  , &OctFilesModel::slotDoubleClicked);
	connect(listviewFiles  , &QListView::activated     , modelFiles  , &OctFilesModel::slotClicked      );
	
	connect(listviewOctData, &QTableView::clicked      , modelOctData, &OctDataModel ::slotClicked      );
	connect(listviewOctData, &QTableView::doubleClicked, modelOctData, &OctDataModel ::slotDoubleClicked);
	connect(listviewOctData, &QTableView::activated    , modelOctData, &OctDataModel ::slotClicked      );

}
