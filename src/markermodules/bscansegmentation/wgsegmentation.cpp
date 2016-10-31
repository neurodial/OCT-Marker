#include "wgsegmentation.h"

#include "bscansegmentation.h"

#include <QButtonGroup>

WGSegmentation::WGSegmentation(::BScanSegmentation* parent)
: segmentation(parent)
{
	setupUi(this);

	QButtonGroup* localMethodBG = new QButtonGroup(this);
	localMethodBG->addButton(radioLocalThreshold);
	localMethodBG->addButton(radioLocalPaint    );
	localMethodBG->addButton(radioLocalOperation);

	QButtonGroup* localThresholdDirectionBG = new QButtonGroup(this);
	localThresholdDirectionBG->addButton(buttonLocalThresholdUp   );
	localThresholdDirectionBG->addButton(buttonLocalThresholdDown );
	localThresholdDirectionBG->addButton(buttonLocalThresholdLeft );
	localThresholdDirectionBG->addButton(buttonLocalThresholdRight);

	createConnections();
}


WGSegmentation::~WGSegmentation()
{
}


void WGSegmentation::createConnections()
{

	connect(buttonSeriesInitFromSeg      , &QPushButton::pressed, this, &WGSegmentation::slotSeriesInitFromSeg   );
	connect(buttonSeriesInitFromThreshold, &QPushButton::pressed, this, &WGSegmentation::slotSeriesInitFromThresh);

	connect(buttonBScanInitFromThreshold , &QPushButton::pressed, this, &WGSegmentation::slotBscanInitFromThresh );
	connect(buttonBScanErode             , &QPushButton::pressed, this, &WGSegmentation::slotBscanErode          );
	connect(buttonBScanDilate            , &QPushButton::pressed, this, &WGSegmentation::slotBscanDilate         );
	connect(buttonBScanOpenClose         , &QPushButton::pressed, this, &WGSegmentation::slotBscanOpenClose      );
	connect(buttonBScanMedian            , &QPushButton::pressed, this, &WGSegmentation::slotBscanMedian         );


	// set local threshold
	connect(buttonLocalThresholdUp       , &QPushButton::toggled, radioLocalThreshold, &QRadioButton::setChecked);
	connect(buttonLocalThresholdDown     , &QPushButton::toggled, radioLocalThreshold, &QRadioButton::setChecked);
	connect(buttonLocalThresholdLeft     , &QPushButton::toggled, radioLocalThreshold, &QRadioButton::setChecked);
	connect(buttonLocalThresholdRight    , &QPushButton::toggled, radioLocalThreshold, &QRadioButton::setChecked);

}



void WGSegmentation::slotSeriesInitFromSeg()
{
}

void WGSegmentation::slotSeriesInitFromThresh()
{
}




void WGSegmentation::slotBscanDilate()
{
}

void WGSegmentation::slotBscanErode()
{
}

void WGSegmentation::slotBscanInitFromThresh()
{
}

void WGSegmentation::slotBscanMedian()
{
}

void WGSegmentation::slotBscanOpenClose()
{
}



void WGSegmentation::slotLocalThresh()
{
}

void WGSegmentation::slotLocalPaint()
{
}

void WGSegmentation::slotLocalOperation()
{
}

