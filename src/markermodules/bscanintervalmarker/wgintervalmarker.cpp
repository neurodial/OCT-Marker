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

#include "wgintervalmarker.h"

#include <algorithm>

#include <QPushButton>
#include <QToolButton>
#include <QVBoxLayout>
#include <QToolBox>
#include <QButtonGroup>
#include <QSignalMapper>
#include <QFileDialog>
#include <QSlider>
#include <QLabel>

#include<data_structure/programoptions.h>

#include <helper/actionclasses.h>
#include <widgets/numberpushbutton.h>

#include "bscanintervalmarker.h"
#include "definedintervalmarker.h"



namespace
{
	QIcon createColorIcon(const QColor& color)
	{
		QPixmap pixmap(15, 15);
		pixmap.fill(color);
		return QIcon(pixmap);
	}

	QSlider* getTransparencySlider(BScanIntervalMarker* parent)
	{
		QSlider* slider = new QSlider;
		slider->setMaximum(255);
		slider->setValue(parent->getTransparency());
		slider->setOrientation(Qt::Horizontal);

		BscanMarkerBase::connect(slider, &QSlider::valueChanged, parent, &BScanIntervalMarker::setTransparency);

		return slider;
	}
}



WGIntervalMarker::WGIntervalMarker(BScanIntervalMarker* parent)
: parent(parent)
, toolboxCollections(new QToolBox(this))
{

	for(const auto& obj : DefinedIntervalMarker::getInstance().getIntervallMarkerMap())
	{
		addMarkerCollection(obj.second);
	}

	QVBoxLayout* layout = new QVBoxLayout();
	layout->addWidget(createMarkerToolButtons());
	layout->addWidget(createTransparencySlider());
	layout->addWidget(toolboxCollections);
	setLayout(layout);

	connect(toolboxCollections, &QToolBox::currentChanged, this, &WGIntervalMarker::changeIntervalCollection);

	connect(parent, &BScanIntervalMarker::markerIdChanged        , this, &WGIntervalMarker::changeMarkerId        );
	connect(parent, &BScanIntervalMarker::markerCollectionChanged, this, &WGIntervalMarker::changeMarkerCollection);

}


WGIntervalMarker::~WGIntervalMarker()
{
}


QWidget* WGIntervalMarker::createTransparencySlider()
{
	QWidget* widget = new QWidget(this);
	QHBoxLayout* layout = new QHBoxLayout();
	layout->addWidget(new QLabel(tr("alpha channel")));
	layout->addWidget(getTransparencySlider(parent));
	widget->setLayout(layout);
	return widget;
}


namespace
{
	QToolButton* createActionToolButton(QWidget* parent, QAction* action)
	{
		QToolButton* button = new QToolButton(parent);
		button->setDefaultAction(action);
		return button;
	}
}


QWidget* WGIntervalMarker::createMarkerToolButtons()
{
	QWidget* widget = new QWidget(this);
	QHBoxLayout* layout = new QHBoxLayout(this);

	for(QAction* action : parent->getMarkerMethodActions())
		layout->addWidget(createActionToolButton(this, action));

	layout->addStretch();

	layout->addWidget(createActionToolButton(this, ProgramOptions::intervallMarkSloMapAuteGenerate.getAction()));

	QAction* exportMarkerToBin = new QAction(this);
	exportMarkerToBin->setText(tr("Export marker to bin file"));
	exportMarkerToBin->setIcon(QIcon(":/icons/disk.png"));
	connect(exportMarkerToBin, &QAction::triggered, this, &WGIntervalMarker::exportMarkerToBinSlot);
	layout->addWidget(createActionToolButton(this, exportMarkerToBin));

	QAction* importMarkerFromBin = new QAction(this);
	importMarkerFromBin->setText(tr("Import marker from bin file"));
	importMarkerFromBin->setIcon(QIcon(":/icons/folder_image.png"));
	connect(importMarkerFromBin, &QAction::triggered, this, &WGIntervalMarker::importMarkerFromBinSlot);
	layout->addWidget(createActionToolButton(this, importMarkerFromBin));


	widget->setLayout(layout);
	return widget;
}





void WGIntervalMarker::addMarkerCollection(const IntervalMarker& markers)
{
	QSignalMapper* signalMapper = new QSignalMapper(this);
	QButtonGroup*  buttonGroup  = new QButtonGroup(this);


	QVBoxLayout* layout = new QVBoxLayout();

	int markerId = 0;
	for(const IntervalMarker::Marker& marker : markers.getIntervalMarkerList())
	{
		QIcon icon = createColorIcon(QColor::fromRgb(marker.getRed(), marker.getGreen(), marker.getBlue()));

		QString buttonText = QString::fromStdString(marker.getName());
		if(!marker.isDefined())
			buttonText = tr("remove mark");

		QPushButton* button = new NumberPushButton(markerId, this);
		button->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
		button->setIcon(icon);
		button->setText(buttonText);
		button->setCheckable(true);
		button->setStyleSheet("Text-align:left; padding:3px;");

        connect(button, &QPushButton::clicked, signalMapper, static_cast<void (QSignalMapper::*)()>(&QSignalMapper::map));
        signalMapper->setMapping(button, markerId);

		buttonGroup->addButton(button);
		buttonGroup->setId(button, markerId);

		layout->addWidget(button);
		++markerId;
	}
	layout->addStretch();

    connect(signalMapper, static_cast<void (QSignalMapper::*)(int)>(&QSignalMapper::mapped), parent, &BScanIntervalMarker::chooseMarkerID);

	QWidget* widget = new QWidget(this);
	widget->setLayout(layout);

	toolboxCollections->addItem(widget, QString::fromStdString(markers.getViewName()));


	collectionsInternalNames.push_back(markers.getInternalName());
	collectionsButtonGroups .push_back(buttonGroup);
}



void WGIntervalMarker::changeIntervalCollection(std::size_t index)
{
	if(index < collectionsInternalNames.size())
	{
		parent->setMarkerCollection(collectionsInternalNames[index]);
		parent->chooseMarkerID(collectionsButtonGroups[index]->checkedId());
	}
}

void WGIntervalMarker::changeMarkerId(std::size_t index)
{
	int actCollectionIndex = toolboxCollections->currentIndex();
	if(actCollectionIndex >= 0 && collectionsButtonGroups.size() > static_cast<std::size_t>(actCollectionIndex))
	{
		QButtonGroup* buttonGroup = collectionsButtonGroups[static_cast<std::size_t>(actCollectionIndex)];
		QAbstractButton* button = buttonGroup->button(static_cast<int>(index));
		if(button)
			button->setChecked(true);
	}
}

void WGIntervalMarker::changeMarkerCollection(const std::string& internalName)
{
	int actToolBoxTab = toolboxCollections->currentIndex();

	if(actToolBoxTab >= 0)
	{
		if(static_cast<std::size_t>(actToolBoxTab) >= collectionsInternalNames.size())
		{
			qDebug("internal Error: WGIntervalMarker::changeMarkerCollection: actToolBoxTab >= collectionsInternalNames.size() | %d >= %d", actToolBoxTab, static_cast<int>(collectionsInternalNames.size()));
			return;
		}

		if(internalName == collectionsInternalNames[static_cast<std::size_t>(actToolBoxTab)])
			return;
	}

	auto pos = std::find(collectionsInternalNames.begin(), collectionsInternalNames.end(), internalName);
	if(pos != collectionsInternalNames.end())
		toolboxCollections->setCurrentIndex(static_cast<int>(pos - collectionsInternalNames.begin()));
}

void WGIntervalMarker::importMarkerFromBinSlot()
{
	QString file = QFileDialog::getOpenFileName(this, tr("Import marker from bin file"), QString(), "*.bin");
	if(!file.isEmpty())
	{
		parent->importMarkerFromBin(file.toStdString());
	}
}


void WGIntervalMarker::exportMarkerToBinSlot()
{
	QString file = QFileDialog::getSaveFileName(this, tr("Export marker to bin file"), QString(), "*.bin");
	if(!file.isEmpty())
	{
		parent->exportMarkerToBin(file.toStdString());
	}
}


