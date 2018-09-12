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

	connect(listviewFiles  , &QListView::clicked         , modelFiles  , &OctFilesModel::slotClicked      );
	connect(listviewFiles  , &QListView::doubleClicked   , modelFiles  , &OctFilesModel::slotDoubleClicked);
	connect(listviewFiles  , &QListView::activated       , modelFiles  , &OctFilesModel::slotClicked      );

	connect(listviewOctData, &QTableView::clicked        , modelOctData, &OctDataModel ::slotClicked      );
	connect(listviewOctData, &QTableView::doubleClicked  , modelOctData, &OctDataModel ::slotDoubleClicked);
	connect(listviewOctData, &QTableView::activated      , modelOctData, &OctDataModel ::slotClicked      );

	connect(modelFiles     , &OctFilesModel::fileIdLoaded, this        , &WGOctDataTree::setSelectFileNum );
}


void WGOctDataTree::setSelectFileNum(QModelIndex index)
{
	listviewFiles->selectionModel()->select(index, QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows);
	listviewFiles->scrollTo(index);
}
