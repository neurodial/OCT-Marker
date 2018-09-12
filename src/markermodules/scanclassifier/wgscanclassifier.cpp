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

#include "wgscanclassifier.h"

#include <QVBoxLayout>
#include <QGroupBox>
#include <QToolBox>
#include <QPushButton>
#include <QToolButton>

#include"classifiermarkerproxy.h"

#include<qt/flowlayout.h>

WGScanClassifier::WGScanClassifier(ScanClassifier* parent)
{
	QVBoxLayout* layout = new QVBoxLayout();
	addProxys2Layout(parent->getScanClassifierProxys(), *layout);

	addProxys2Layout(parent->getBScanClassifierProxys(), *layout);

	layout->addStretch();
	setLayout(layout);
}

WGScanClassifier::~WGScanClassifier()
{
}

QAbstractButton * WGScanClassifier::genButton(QAction* action)
{
	QToolButton* button = new QToolButton(this);
	button->setDefaultAction(action);
	return button;
}

void WGScanClassifier::addProxys2Layout(ScanClassifier::ClassifierProxys& proxyList, QLayout& layout)
{
	for(ClassifierMarkerProxy* proxy : proxyList)
	{
		if(!proxy)
			continue;

		QGroupBox  * classifierGroup       = new QGroupBox(QString::fromStdString(proxy->getClassifierMarker().getViewName()), this);
// 		QVBoxLayout* classifierGroupLayout = new QVBoxLayout();
		FlowLayout* classifierGroupLayout = new FlowLayout();

		for(QAction* action : proxy->getMarkerActions())
			classifierGroupLayout->addWidget(genButton(action));

		classifierGroup->setLayout(classifierGroupLayout);
		layout.addWidget(classifierGroup);
	}
}
