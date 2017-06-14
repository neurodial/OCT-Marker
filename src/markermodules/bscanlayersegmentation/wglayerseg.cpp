#include "wglayerseg.h"

#include<QVBoxLayout>
#include<QButtonGroup>
#include<QPushButton>
#include<QSignalMapper>

#include<octdata/datastruct/segmentationlines.h>

#include "bscanlayersegmentation.h"

WGLayerSeg::WGLayerSeg(BScanLayerSegmentation* parent)
: parent(parent)
{

	layerButtons = new QButtonGroup(this);

	QVBoxLayout* layout = new QVBoxLayout();
	QSignalMapper* signalMapper = new QSignalMapper(this);

	for(OctData::Segmentationlines::SegmentlineType type : OctData::Segmentationlines::getSegmentlineTypes())
	{
		QPushButton* button = new QPushButton(OctData::Segmentationlines::getSegmentlineName(type));
		button->setCheckable(true);

        connect(button, &QPushButton::clicked, signalMapper, static_cast<void (QSignalMapper::*)()>(&QSignalMapper::map));
        signalMapper->setMapping(button, static_cast<int>(type));

		layerButtons->addButton(button);
		layout->addWidget(button);
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
	qDebug("Segline: %d", static_cast<int>(index));

	std::size_t numSegLines = OctData::Segmentationlines::getSegmentlineTypes().size();
	if(index < numSegLines)
		parent->setActEditLinetype(OctData::Segmentationlines::getSegmentlineTypes().at(index));
}

