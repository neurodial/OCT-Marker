#include "wgintervalmarker.h"

#include <QScrollArea>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QToolBox>
#include <QSignalMapper>

#include <helper/actionclasses.h>

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
}



WGIntervalMarker::WGIntervalMarker(BScanIntervalMarker* parent)
: parent(parent)
{

	QToolBox* toolbox = new QToolBox(this);

	for(const auto& obj : DefinedIntervalMarker::getInstance().getIntervallMarkerMap())
	{
		addMarkerCollection(obj.second, toolbox);
	}

	QVBoxLayout* layout = new QVBoxLayout();
	layout->addWidget(toolbox);
	setLayout(layout);

	connect(toolbox, &QToolBox::currentChanged, this, &WGIntervalMarker::changeIntervalCollection);

}


WGIntervalMarker::~WGIntervalMarker()
{
}




void WGIntervalMarker::addMarkerCollection(const IntervalMarker& markers, QToolBox* toolbox)
{
	QSignalMapper* signalMapper = new QSignalMapper(this);

	QScrollArea* scrollArea = new QScrollArea;
	QVBoxLayout* layout = new QVBoxLayout();
	scrollArea->setLayout(layout);

	for(const IntervalMarker::Marker& marker : markers.getIntervalMarkerList())
	{
		std::size_t markerId = marker.getInternalId();
		QIcon icon = createColorIcon(QColor::fromRgb(marker.getRed(), marker.getGreen(), marker.getBlue()));

// 		SizetValueAction* markerAction = new SizetValueAction(markerId, this);
// 		// markerAction->setCheckable(true);
// 		markerAction->setText(QString::fromStdString(marker.getName()));
// 		markerAction->setIcon(icon);
// 		markerAction->setChecked(actMarkerId == markerId);
// 		connect(markerAction, &SizetValueAction::triggered         , parent      , &BScanIntervalMarker::chooseMarkerID);
// 		connect(parent      , &BScanIntervalMarker::markerIdChanged, markerAction, &SizetValueAction::valueChanged     );
//
// 		actionGroupMarker.addAction(markerAction);
// 		markerToolbar.addAction(markerAction);
//
// 		actionList.push_back(markerAction);

// 		QLabel* testlabel = new QLabel();
		QPushButton* button = new QPushButton(icon, QString::fromStdString(marker.getName()), this);
// 		connect(this, &QPushButton::clicked, markerAction, &SizetValueAction::trigger);
        connect(button, &QPushButton::clicked, signalMapper, static_cast<void (QSignalMapper::*)()>(&QSignalMapper::map));
        signalMapper->setMapping(button, markerId);
// 		button->setStyleSheet("Text-align:left");

		layout->addWidget(button);
	}
	layout->addStretch();


    connect(signalMapper, static_cast<void (QSignalMapper::*)(int)>(&QSignalMapper::mapped), parent, &BScanIntervalMarker::chooseMarkerID);

	QWidget* wgCollection = new QWidget(this);
	QVBoxLayout* wgLayout = new QVBoxLayout();
	wgCollection->setLayout(wgLayout);
	wgLayout->addWidget(scrollArea);

	toolbox->addItem(wgCollection, QString::fromStdString(markers.getViewName()));

	collectionsInternalNames.push_back(markers.getInternalName());
}



void WGIntervalMarker::changeIntervalCollection(std::size_t index)
{
	if(index < collectionsInternalNames.size())
	{
		parent->setMarkerCollection(collectionsInternalNames[index]);
	}
}

