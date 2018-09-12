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

#include "bscansegtoolbar.h"

#include <QSpinBox>

#include<data_structure/programoptions.h>
#include "bscansegmentation.h"

BScanSegToolBar::BScanSegToolBar(BScanSegmentation* seg, QObject* parent)
: QToolBar(tr("Segmentation"), dynamic_cast<QWidget*>(parent))
{
	setObjectName("ToolBarSegmentationMarker");

	addWidget(ProgramOptions::freeFormedSegmetationLineThickness.createSpinBox(this));

	QAction* actionErodeBScan = new QAction(this);
	actionErodeBScan->setText(tr("Erode"));
	actionErodeBScan->setIcon(QIcon(":/icons/arrow_in.png"));
	connect(actionErodeBScan, &QAction::triggered, seg, &BScanSegmentation::erodeBScan);
	addAction(actionErodeBScan);


	QAction* actionDilateBScan = new QAction(this);
	actionDilateBScan->setText(tr("Dilate"));
	actionDilateBScan->setIcon(QIcon(":/icons/arrow_out.png"));
	connect(actionDilateBScan, &QAction::triggered, seg, &BScanSegmentation::dilateBScan);
	addAction(actionDilateBScan);

	QAction* actionOpenCloseBScan = new QAction(this);
	actionOpenCloseBScan->setText(tr("Open/Close"));
	actionOpenCloseBScan->setIcon(QIcon(":/icons/arrow_inout.png"));
	connect(actionOpenCloseBScan, &QAction::triggered, seg, &BScanSegmentation::opencloseBScan);
	addAction(actionOpenCloseBScan);

	QAction* actionMedianBScan = new QAction(this);
	actionMedianBScan->setText(tr("Median"));
	actionMedianBScan->setIcon(QIcon(":/icons/arrow_inout.png"));
	connect(actionMedianBScan, &QAction::triggered, seg, &BScanSegmentation::medianBScan);
	addAction(actionMedianBScan);

	QAction* actionCutUnconectedAreas = new QAction(this);
	actionCutUnconectedAreas->setText(tr("remove unconected areas"));
	actionCutUnconectedAreas->setIcon(QIcon(":/icons/cut_red.png"));
	connect(actionCutUnconectedAreas, &QAction::triggered, seg, &BScanSegmentation::removeUnconectedAreas);
	addAction(actionCutUnconectedAreas);

	QAction* actionExtendLeftRightSpace = new QAction(this);
	actionExtendLeftRightSpace->setText(tr("extend segmentation left and right"));
	actionExtendLeftRightSpace->setIcon(QIcon(":/icons/tag.png"));
	connect(actionExtendLeftRightSpace, &QAction::triggered, seg, &BScanSegmentation::extendLeftRightSpace);
	addAction(actionExtendLeftRightSpace);

}
