#include "wglayerseg.h"

#include "bscanlayersegmentation.h"

#include<QVBoxLayout>
#include<QButtonGroup>
#include<QPushButton>
#include<QSignalMapper>
#include<QActionGroup>
#include<QAction>
#include<QToolButton>
#include<QComboBox>
#include<QLabel>
#include<QDesktopWidget>

#include<octdata/datastruct/segmentationlines.h>

#include<widgets/numberpushbutton.h>

#include"thicknessmaptemplates.h"
#include <data_structure/programoptions.h>

namespace
{
	class LayoutAdder
	{
		QLayout* basisLayout;
		QGridLayout* gridLayout = nullptr;
		QWidget* layoutWidget = nullptr;
		int numWidgets = 0;
	public:
		LayoutAdder(QLayout* layout, QWidget* parent)
		: basisLayout(layout)
		{
			QDesktopWidget desktop;
			QRect rect = desktop.availableGeometry();
			if(rect.height() < 1000)
			{
				layoutWidget = new QWidget(parent);
				gridLayout = new QGridLayout(layoutWidget);
			}
		}

		~LayoutAdder()
		{
			if(layoutWidget)
				basisLayout->addWidget(layoutWidget);
		}

		void addWidget(QWidget* widget)
		{
			if(gridLayout)
			{
				gridLayout->addWidget(widget, numWidgets/2, numWidgets % 2);
				++numWidgets;
			}
			else
				basisLayout->addWidget(widget);
		}
	};
}


WGLayerSeg::WGLayerSeg(BScanLayerSegmentation* parent)
: parent(parent)
, thicknessmapTemplates(new QComboBox(this))
{

	QVBoxLayout* layout = new QVBoxLayout();


	addThicknessMapControls(*layout);
	createMarkerToolButtons(*layout);

	addLayerButtons(*layout);

	layout->addStretch();
	setLayout(layout);


	connect(parent, &BScanLayerSegmentation::segMethodChanged     , this, &WGLayerSeg::markerMethodChanged  );
	connect(parent, &BScanLayerSegmentation::segLineIdChanged     , this, &WGLayerSeg::segLineIdChanged     );
	connect(parent, &BScanLayerSegmentation::segLineVisibleChanged, this, &WGLayerSeg::segLineVisibleChanged);
	connect(thicknessmapTemplates, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &WGLayerSeg::thicknessmapTemplateChanged);

	connect(&ProgramOptions::layerSegSloMapsAutoUpdate, &OptionBool::valueChangedInvers, actionUpdateThicknessmap, &QAction::setEnabled);
	actionUpdateThicknessmap->setEnabled(!ProgramOptions::layerSegSloMapsAutoUpdate());
}


WGLayerSeg::~WGLayerSeg()
{
}


void WGLayerSeg::addLayerButtons(QLayout& layout)
{
	LayoutAdder layoutAdder(&layout, this);

	layerButtons = new QButtonGroup(this);
	QSignalMapper* signalMapper = new QSignalMapper(this);

	const std::size_t numSegLines = OctData::Segmentationlines::getSegmentlineTypes().size();
	seglineButtons.resize(numSegLines);

	const OctData::Segmentationlines::SegmentlineType actType = parent->getActEditSeglineType();
	for(OctData::Segmentationlines::SegmentlineType type : OctData::Segmentationlines::getSegmentlineTypes())
	{
		const auto keyListIt = std::find(BScanLayerSegmentation::keySeglines.begin(), BScanLayerSegmentation::keySeglines.end(), type);

		QPushButton* button;
		if(keyListIt != BScanLayerSegmentation::keySeglines.end())
		{
			const int pos = static_cast<int>(keyListIt - BScanLayerSegmentation::keySeglines.begin());
			button = new NumberPushButton(pos, this);
		}
		else
			button = new QPushButton(this);

		button->setText(OctData::Segmentationlines::getSegmentlineName(type));
		button->setCheckable(true);

        connect(button, &QPushButton::clicked, signalMapper, static_cast<void (QSignalMapper::*)()>(&QSignalMapper::map));
        signalMapper->setMapping(button, static_cast<int>(type));

		if(actType == type)
			button->setChecked(true);

		layerButtons->addButton(button);
		layoutAdder.addWidget(button);
		seglineButtons[static_cast<std::size_t>(type)] = button;
	}
    connect(signalMapper, static_cast<void (QSignalMapper::*)(int)>(&QSignalMapper::mapped), this, &WGLayerSeg::changeSeglineId);
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



void WGLayerSeg::createMarkerToolButtons(QLayout& layout)
{
	QWidget* widget = new QWidget(this);
	QHBoxLayout* layoutTools = new QHBoxLayout(widget);

	actionMarkerMethodPen = new QAction(this);
	actionMarkerMethodPen->setText(tr("pen"));
	actionMarkerMethodPen->setIcon(QIcon(":/icons/typicons/pencil.svg"));
// 	actionMarkerMethodPen->setIcon(QIcon(":/icons/pen.png"));
	actionMarkerMethodPen->setCheckable(true);
	connect(actionMarkerMethodPen, &QAction::triggered, this, &WGLayerSeg::setMarkerMethodPen);
	buttonMarkerMethodPen = createActionToolButton(this, actionMarkerMethodPen);
	layoutTools->addWidget(buttonMarkerMethodPen);

	actionMarkerMethodSpline = new QAction(this);
	actionMarkerMethodSpline->setText(tr("Spline"));
	actionMarkerMethodSpline->setIcon(QIcon(":/icons/typicons/spline.svg"));
// 	actionMarkerMethodSpline->setIcon(QIcon(":/icons/vector.png"));
	actionMarkerMethodSpline->setCheckable(true);
	connect(actionMarkerMethodSpline, &QAction::triggered, this, &WGLayerSeg::setMarkerMethodSpline);
	buttonMarkerMethodSpline = createActionToolButton(this, actionMarkerMethodSpline);
	layoutTools->addWidget(buttonMarkerMethodSpline);


	QActionGroup* alignmentGroup = new QActionGroup(this);
	alignmentGroup->addAction(actionMarkerMethodPen);
	alignmentGroup->addAction(actionMarkerMethodSpline);

	markerMethodChanged();

	layoutTools->addStretch();

	actionShowSeglines = new QAction(this);
	actionShowSeglines->setText(tr("show segmentationlines"));
	actionShowSeglines->setIcon(QIcon(":/icons/own/seglines.svg"));
	actionShowSeglines->setCheckable(true);
	actionShowSeglines->setChecked(parent->isSegmentationLinesVisible());
	connect(actionShowSeglines, &QAction::triggered, parent, &BScanLayerSegmentation::setSegmentationLinesVisible);
	buttonShowSeglines = createActionToolButton(this, actionShowSeglines);
	layoutTools->addWidget(buttonShowSeglines);

	widget->setLayout(layoutTools);
	layout.addWidget(widget);
}


void WGLayerSeg::addThicknessMapControls(QLayout& layout)
{
	QWidget* widgetTools = new QWidget(this);
	QHBoxLayout* layoutTools = new QHBoxLayout(widgetTools);

	layoutTools->addWidget(new QLabel(tr("Thicknessmap")));

	ThicknessmapTemplates& templates = ThicknessmapTemplates::getInstance();
	thicknessmapTemplates->addItem(tr("None"), QVariant::fromValue(static_cast<void*>(nullptr)));
	for(const ThicknessmapTemplates::Configuration& config : templates.getConfigurations())
	{
		QString text = QString("%1 (%2 - %3)").arg(config.getName())
		                                      .arg(OctData::Segmentationlines::getSegmentlineName(config.getLine1()))
		                                      .arg(OctData::Segmentationlines::getSegmentlineName(config.getLine2()));
		thicknessmapTemplates->addItem(text, QVariant::fromValue(const_cast<void*>(static_cast<const void*>(&config))));
	}
	int width = thicknessmapTemplates->minimumSizeHint().width();
	thicknessmapTemplates->setMinimumWidth(width/2);

	layoutTools->addWidget(thicknessmapTemplates);

	layoutTools->addStretch();

	actionUpdateThicknessmap = new QAction(this);
	actionUpdateThicknessmap->setText(tr("update thicknessmap"));
	actionUpdateThicknessmap->setIcon(QIcon(":/icons/typicons/arrow-sync-outline.svg"));
	connect(actionUpdateThicknessmap, &QAction::triggered, parent, &BScanLayerSegmentation::generateThicknessmap);
	QToolButton* buttonUpdateThicknessmap = createActionToolButton(this, actionUpdateThicknessmap);
	layoutTools->addWidget(buttonUpdateThicknessmap);

	layoutTools->addWidget(createActionToolButton(this, ProgramOptions::layerSegThicknessmapBlend.getAction()));

	layout.addWidget(widgetTools);
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
	buttonMarkerMethodPen   ->setIconSize(iconSize);
	buttonMarkerMethodSpline->setIconSize(iconSize);
	buttonShowSeglines      ->setIconSize(iconSize);
}

void WGLayerSeg::thicknessmapTemplateChanged(int indexInt)
{
	if(indexInt == 0) // none thicknessmap
	{
		parent->setThicknessmapVisible(false);
		return;
	}

	const std::size_t index = static_cast<std::size_t>(indexInt-1);
	ThicknessmapTemplates& templates = ThicknessmapTemplates::getInstance();
	const std::vector<ThicknessmapTemplates::Configuration>& configurations = templates.getConfigurations();
	if(index < configurations.size())
		parent->setThicknessmapConfig(configurations[index]);
}


void WGLayerSeg::segLineIdChanged(std::size_t index)
{
	if(seglineButtons.size() > index)
	{
		seglineButtons[index]->setChecked(true);
	}
}

void WGLayerSeg::segLineVisibleChanged(bool v)
{
	actionShowSeglines->setChecked(v);
}

