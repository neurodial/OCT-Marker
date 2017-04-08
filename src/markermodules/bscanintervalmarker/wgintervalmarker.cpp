#include "wgintervalmarker.h"

#include <QScrollArea>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QToolBox>
#include <QButtonGroup>
#include <QSignalMapper>

#include <helper/actionclasses.h>

#include "bscanintervalmarker.h"
#include "definedintervalmarker.h"
#include <QToolButton>

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

	connect(parent, &BScanIntervalMarker::markerIdChanged, this, &WGIntervalMarker::changeMarkerId);

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
/*
	QActionGroup*  actionGroupMethod  = new QActionGroup(this);

	BScanIntervalMarker::Method markerMethod = parent->getMarkerMethod();

	IntValueAction* paintMarkerAction = new IntValueAction(static_cast<int>(BScanIntervalMarker::Method::Paint), this);
	paintMarkerAction->setCheckable(true);
	paintMarkerAction->setText(tr("paint marker"));
	paintMarkerAction->setIcon(QIcon(":/icons/paintbrush.png"));
	paintMarkerAction->setChecked(markerMethod == BScanIntervalMarker::Method::Paint);
	connect(paintMarkerAction, &IntValueAction::triggered               , parent           , static_cast<void(BScanIntervalMarker::*)(int)>(&BScanIntervalMarker::chooseMethodID));
	connect(parent           , &BScanIntervalMarker::markerMethodChanged, paintMarkerAction, &IntValueAction::valueChanged        );
	actionGroupMethod->addAction(paintMarkerAction);

	IntValueAction* fillMarkerAction = new IntValueAction(static_cast<int>(BScanIntervalMarker::Method::Fill), this);
	fillMarkerAction->setCheckable(true);
	fillMarkerAction->setText(tr("fill marker"));
	fillMarkerAction->setIcon(QIcon(":/icons/paintcan.png"));
	fillMarkerAction->setChecked(markerMethod == BScanIntervalMarker::Method::Fill);
	connect(fillMarkerAction, &IntValueAction::triggered               , parent          , static_cast<void(BScanIntervalMarker::*)(int)>(&BScanIntervalMarker::chooseMethodID));
	connect(parent          , &BScanIntervalMarker::markerMethodChanged, fillMarkerAction, &IntValueAction::valueChanged        );
	actionGroupMethod->addAction(fillMarkerAction);


	actionGroupMethod->setExclusive(true);


	layout->addWidget(createActionToolButton(this, paintMarkerAction));
	layout->addWidget(createActionToolButton(this, fillMarkerAction));*/

	layout->addStretch();
	widget->setLayout(layout);
	return widget;
}





void WGIntervalMarker::addMarkerCollection(const IntervalMarker& markers)
{
	QSignalMapper* signalMapper = new QSignalMapper(this);
	QButtonGroup*  buttonGroup  = new QButtonGroup(this);


	QScrollArea* scrollArea = new QScrollArea;
	QVBoxLayout* layout = new QVBoxLayout();
	scrollArea->setLayout(layout);

	int markerId = 0;
	for(const IntervalMarker::Marker& marker : markers.getIntervalMarkerList())
	{
		QIcon icon = createColorIcon(QColor::fromRgb(marker.getRed(), marker.getGreen(), marker.getBlue()));

		QString buttonText = QString::fromStdString(marker.getName());
		if(!marker.isDefined())
			buttonText = tr("remove mark");

		QPushButton* button = new QPushButton(icon, buttonText, this);
        connect(button, &QPushButton::clicked, signalMapper, static_cast<void (QSignalMapper::*)()>(&QSignalMapper::map));
        signalMapper->setMapping(button, markerId);
// 		button->setStyleSheet("Text-align:left");
		button->setCheckable(true);

		buttonGroup->addButton(button);
		buttonGroup->setId(button, markerId);

		layout->addWidget(button);
		++markerId;
	}
	layout->addStretch();


    connect(signalMapper, static_cast<void (QSignalMapper::*)(int)>(&QSignalMapper::mapped), parent, &BScanIntervalMarker::chooseMarkerID);

	QWidget* wgCollection = new QWidget(this);
	QVBoxLayout* wgLayout = new QVBoxLayout();
	wgCollection->setLayout(wgLayout);
	wgLayout->addWidget(scrollArea);

	toolboxCollections->addItem(wgCollection, QString::fromStdString(markers.getViewName()));

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

