#include "wgintervalmarker.h"

#include <QPushButton>
#include <QToolButton>
#include <QVBoxLayout>
#include <QToolBox>
#include <QButtonGroup>
#include <QSignalMapper>

#include <helper/actionclasses.h>
#include <widgets/numberpushbutton.h>

#include "bscanintervalmarker.h"
#include "definedintervalmarker.h"


#include <algorithm>

namespace
{
	QIcon createColorIcon(const QColor& color)
	{
		QPixmap pixmap(15, 15);
		pixmap.fill(color);
		return QIcon(pixmap);
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
	layout->addWidget(toolboxCollections);
	setLayout(layout);

	connect(toolboxCollections, &QToolBox::currentChanged, this, &WGIntervalMarker::changeIntervalCollection);

	connect(parent, &BScanIntervalMarker::markerIdChanged        , this, &WGIntervalMarker::changeMarkerId        );
	connect(parent, &BScanIntervalMarker::markerCollectionChanged, this, &WGIntervalMarker::changeMarkerCollection);

}


WGIntervalMarker::~WGIntervalMarker()
{
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

