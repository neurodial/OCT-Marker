#include "wglayerseg.h"

#include "bscanlayersegmentation.h"

#include<QVBoxLayout>
#include<QButtonGroup>
#include<QPushButton>
#include<QSignalMapper>
#include<QActionGroup>
#include<QAction>
#include<QToolButton>

#include<octdata/datastruct/segmentationlines.h>


WGLayerSeg::WGLayerSeg(BScanLayerSegmentation* parent)
: parent(parent)
{
	connect(parent, &BScanLayerSegmentation::segMethodChanged, this, &WGLayerSeg::markerMethodChanged);

	QVBoxLayout* layout = new QVBoxLayout();
	layout->addWidget(createMarkerToolButtons());


	layerButtons = new QButtonGroup(this);

	QSignalMapper* signalMapper = new QSignalMapper(this);

	const std::size_t numSegLines = OctData::Segmentationlines::getSegmentlineTypes().size();
	seglineButtons.resize(numSegLines);

	const OctData::Segmentationlines::SegmentlineType actType = parent->getActEditSeglineType();

	for(OctData::Segmentationlines::SegmentlineType type : OctData::Segmentationlines::getSegmentlineTypes())
	{
		QPushButton* button = new QPushButton(OctData::Segmentationlines::getSegmentlineName(type));
		button->setCheckable(true);

        connect(button, &QPushButton::clicked, signalMapper, static_cast<void (QSignalMapper::*)()>(&QSignalMapper::map));
        signalMapper->setMapping(button, static_cast<int>(type));

		if(actType == type)
			button->setChecked(true);

		layerButtons->addButton(button);
		layout->addWidget(button);
		seglineButtons[static_cast<std::size_t>(type)] = button;
	}

    connect(signalMapper, static_cast<void (QSignalMapper::*)(int)>(&QSignalMapper::mapped), this, &WGLayerSeg::changeSeglineId);

	layout->addStretch();
	setLayout(layout);
}


WGLayerSeg::~WGLayerSeg()
{
}



void WGLayerSeg::changeSeglineId(std::size_t index)
{
	const std::size_t numSegLines = OctData::Segmentationlines::getSegmentlineTypes().size();
	if(index < numSegLines)
		parent->setActEditLinetype(OctData::Segmentationlines::getSegmentlineTypes().at(index));
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



QWidget* WGLayerSeg::createMarkerToolButtons()
{
	QWidget* widget = new QWidget(this);
	QHBoxLayout* layout = new QHBoxLayout(this);

	actionMarkerMethodPen = new QAction(this);
	actionMarkerMethodPen->setText(tr("pen"));
	actionMarkerMethodPen->setIcon(QIcon(":/icons/typicons/pencil.svg"));
// 	actionMarkerMethodPen->setIcon(QIcon(":/icons/pen.png"));
	actionMarkerMethodPen->setCheckable(true);
	connect(actionMarkerMethodPen, &QAction::triggered, this, &WGLayerSeg::setMarkerMethodPen);
	buttonMarkerMethodPen = createActionToolButton(this, actionMarkerMethodPen);
	layout->addWidget(buttonMarkerMethodPen);

	actionMarkerMethodSpline = new QAction(this);
	actionMarkerMethodSpline->setText(tr("Spline"));
	actionMarkerMethodSpline->setIcon(QIcon(":/icons/typicons/spline.svg"));
// 	actionMarkerMethodSpline->setIcon(QIcon(":/icons/vector.png"));
	actionMarkerMethodSpline->setCheckable(true);
	connect(actionMarkerMethodSpline, &QAction::triggered, this, &WGLayerSeg::setMarkerMethodSpline);
	buttonMarkerMethodSpline = createActionToolButton(this, actionMarkerMethodSpline);
	layout->addWidget(buttonMarkerMethodSpline);


	QActionGroup* alignmentGroup = new QActionGroup(this);
	alignmentGroup->addAction(actionMarkerMethodPen);
	alignmentGroup->addAction(actionMarkerMethodSpline);

	markerMethodChanged();

	layout->addStretch();

	actionShowSeglines = new QAction(this);
	actionShowSeglines->setText(tr("show segmentationlines"));
	actionShowSeglines->setIcon(QIcon(":/icons/own/seglines.svg"));
	actionShowSeglines->setCheckable(true);
	actionShowSeglines->setChecked(parent->isSegmentationLinesVisible());
	connect(actionShowSeglines, &QAction::triggered, parent, &BScanLayerSegmentation::setSegmentationLinesVisible);
	buttonShowSeglines = createActionToolButton(this, actionShowSeglines);
	layout->addWidget(buttonShowSeglines);

	widget->setLayout(layout);
	return widget;
}

void WGLayerSeg::setMarkerMethodPen   () { parent->setSegMethod(BScanLayerSegmentation::SegMethod::Pen   ); }
void WGLayerSeg::setMarkerMethodSpline() { parent->setSegMethod(BScanLayerSegmentation::SegMethod::Spline); }

void WGLayerSeg::markerMethodChanged()
{
	BScanLayerSegmentation::SegMethod method = parent->getSegMethod();

	actionMarkerMethodPen   ->setChecked(method == BScanLayerSegmentation::SegMethod::Pen   );
	actionMarkerMethodSpline->setChecked(method == BScanLayerSegmentation::SegMethod::Spline);

}

void WGLayerSeg::setIconsToSimple(int size)
{
	QSize iconSize(size, size);
	buttonMarkerMethodPen->setIconSize(iconSize);

	buttonMarkerMethodSpline->setIconSize(iconSize);

	buttonShowSeglines->setIconSize(iconSize);
}

