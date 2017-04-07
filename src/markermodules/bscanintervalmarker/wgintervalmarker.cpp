#include "wgintervalmarker.h"

#include <QScrollArea>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QToolBox>
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

}


void WGIntervalMarker::addMarkerCollection(const IntervalMarker& markers, QToolBox* toolbox)
{

	QScrollArea* scrollArea = new QScrollArea;
	QVBoxLayout* layout = new QVBoxLayout();
	scrollArea->setLayout(layout);

	for(const IntervalMarker::Marker& marker : markers.getIntervalMarkerList())
	{
		std::size_t markerId = marker.getInternalId();
		QIcon icon = createColorIcon(QColor::fromRgb(marker.getRed(), marker.getGreen(), marker.getBlue()));

// 		SizetValueAction* markerAction = new SizetValueAction(markerId, parent);
// 		// markerAction->setCheckable(true);
// 		markerAction->setText(QString::fromStdString(marker.getName()));
// 		markerAction->setIcon(icon);
// 		markerAction->setChecked(actMarkerId == markerId);
// 		connect(markerAction, &SizetValueAction::triggered         , this        , &BScanIntervalMarker::chooseMarkerID);
// 		connect(this        , &BScanIntervalMarker::markerIdChanged, markerAction, &SizetValueAction::valueChanged     );
//
// 		actionGroupMarker.addAction(markerAction);
// 		markerToolbar.addAction(markerAction);
//
// 		actionList.push_back(markerAction);

// 		QLabel* testlabel = new QLabel();
		QPushButton* button = new QPushButton(icon, QString::fromStdString(marker.getName()), this);
// 		button->setStyleSheet("Text-align:left");

		layout->addWidget(button);
	}
	layout->addStretch();

	QWidget* wgCollection = new QWidget(this);
	QVBoxLayout* wgLayout = new QVBoxLayout();
	wgCollection->setLayout(wgLayout);
	wgLayout->addWidget(scrollArea);

	toolbox->addItem(wgCollection, QString::fromStdString(markers.getViewName()));
}



WGIntervalMarker::~WGIntervalMarker()
{
}
