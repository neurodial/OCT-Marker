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

#ifndef WGOCTDATATREE_H
#define WGOCTDATATREE_H

#include <QWidget>

class QListView;
class QTableView;
class OctDataModel;
class OctFilesModel;

class WGOctDataTree : public QWidget
{
	Q_OBJECT

	QListView*  listviewFiles = nullptr;
	OctFilesModel* modelFiles = nullptr;
	
	QTableView* listviewOctData = nullptr;
	OctDataModel* modelOctData  = nullptr;

public:
	WGOctDataTree();

private slots:
	void setSelectFileNum(QModelIndex index);

};

#endif // WGOCTDATATREE_H
