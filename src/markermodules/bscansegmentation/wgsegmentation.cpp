#include "wgsegmentation.h"

#include "bscansegmentation.h"
#include "configdata.h"

#include <QButtonGroup>

WGSegmentation::WGSegmentation(BScanSegmentation* parent)
: segmentation(parent)
, thresSeries(this)
, thresBScan (this)
, thresLocal (this)
{
	setupUi(this);


	// Button groups for series
	thresSeries.setButtonUp     (seriesThresholdDirectionUp);
	thresSeries.setButtonDown   (seriesThresholdDirectionDown);
//	thresSeries.setButtonLeft   ();
//	thresSeries.setButtonRight  ();

	thresSeries.setButtonAbsolut(seriesThresholdMethodAbs );
	thresSeries.setButtonRelativ(seriesThresholdMethodRel );

	thresSeries.setAbsoluteBox  (boxSeriesThresAbsolute   );
	thresSeries.setRelativeBox  (boxSeriesThresRelative   );
	thresSeries.setStrikesBox   (boxSeriesThresStrikes    );
	thresSeries.setupWidgets();



	thresBScan.setButtonUp     (radioBScanThresFromBelow);
	thresBScan.setButtonDown   (radioBScanThresFromAbove);
//	thresBScan.setButtonLeft   ();
//	thresBScan.setButtonRight  ();

	thresBScan.setButtonAbsolut(radioBScanThresAbsolute );
	thresBScan.setButtonRelativ(radioBScanThresRelative );

	thresBScan.setAbsoluteBox  (boxBScanThresAbsolute   );
	thresBScan.setRelativeBox  (boxBScanThresRelative   );
	thresBScan.setStrikesBox   (boxBScanThresStrikes    );
	thresBScan.setupWidgets();


	thresLocal.setButtonUp     (buttonLocalThresholdUp   );
	thresLocal.setButtonDown   (buttonLocalThresholdDown );
	thresLocal.setButtonLeft   (buttonLocalThresholdLeft );
	thresLocal.setButtonRight  (buttonLocalThresholdRight);

	thresLocal.setButtonAbsolut(radioLocalThresholdAbsolut);
	thresLocal.setButtonRelativ(radioLocalThresholdRelative);

	thresLocal.setAbsoluteBox  (localThresholdAboluteSpinbox);
	thresLocal.setRelativeBox  (localThresholdRelativeSpinBox);
	thresLocal.setStrikesBox   (localThresholdStrikes);
	thresLocal.setupWidgets();


	// Button groups for local

	QButtonGroup* localMethodBG = new QButtonGroup(this);
	localMethodBG->addButton(radioLocalThreshold);
	localMethodBG->addButton(radioLocalPaint    );
	localMethodBG->addButton(radioLocalOperation);

	QButtonGroup* localPaintMethodBG = new QButtonGroup(this);
	localPaintMethodBG->addButton(buttonLocalPaintCircle);
	localPaintMethodBG->addButton(buttonLocalPaintRec   );

	QButtonGroup* localPaintAreaBG = new QButtonGroup(this);
	localPaintAreaBG->addButton(buttonLocalPaintArea0   );
	localPaintAreaBG->addButton(buttonLocalPaintAreaAuto);
	localPaintAreaBG->addButton(buttonLocalPaintArea1   );

	QButtonGroup* localOperationBG = new QButtonGroup(this);
	localOperationBG->addButton(buttonLocalOperationErode );
	localOperationBG->addButton(buttonLocalOperationDilate);

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
	connect(&thresLocal       , &WGSegmentationThreshold::blockAction, radioLocalThreshold, &QRadioButton::setChecked);
// 	connect(buttonLocalThresholdDown     , &QPushButton::toggled, radioLocalThreshold, &QRadioButton::setChecked);
// 	connect(buttonLocalThresholdLeft     , &QPushButton::toggled, radioLocalThreshold, &QRadioButton::setChecked);
// 	connect(buttonLocalThresholdRight    , &QPushButton::toggled, radioLocalThreshold, &QRadioButton::setChecked);

	//connect(



	connect(localSizeSpinBox, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), segmentation    , &BScanSegmentation::setLocalOperatorSize);
	connect(segmentation    , &BScanSegmentation::localOperatorSizeChanged                , localSizeSpinBox, &QSpinBox::setValue                     );
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
	BScanSegmentationMarker::ThresholdData data;
	thresBScan.getThresholdData(data);
	segmentation->initBScanFromThreshold(data);
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






WGSegmentationThreshold::WGSegmentationThreshold(WGSegmentation* parent)
: widget(parent)
{
}

void WGSegmentationThreshold::setupWidgets()
{
	QButtonGroup* directionBG = new QButtonGroup(widget);

	if(buttonUp   )
		directionBG->addButton(buttonUp   );
	if(buttonDown )
		directionBG->addButton(buttonDown );
	if(buttonLeft )
		directionBG->addButton(buttonLeft );
	if(buttonRight)
		directionBG->addButton(buttonRight);

	if(buttonAbsolut && buttonRelativ)
	{
		QButtonGroup* methodBG = new QButtonGroup(widget);
		methodBG->addButton(buttonAbsolut);
		methodBG->addButton(buttonRelativ);
	}

	connect(absoluteBox, static_cast<void(QSpinBox::*      )(int   )>(&QSpinBox::valueChanged)      , this, &WGSegmentationThreshold::absoluteSpinBoxChanged);
	connect(relativeBox, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &WGSegmentationThreshold::relativeSpinBoxChanged);


	connect(absoluteBox, static_cast<void(QSpinBox::*      )(int   )>(&QSpinBox::valueChanged)      , this, &WGSegmentationThreshold::widgetActivated);
	connect(relativeBox, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &WGSegmentationThreshold::widgetActivated);
	connect(strikesBox , static_cast<void(QSpinBox::*      )(int   )>(&QSpinBox::valueChanged)      , this, &WGSegmentationThreshold::widgetActivated);

	connect(buttonUp     , &QAbstractButton::toggled, this, &WGSegmentationThreshold::widgetActivated);
	connect(buttonDown   , &QAbstractButton::toggled, this, &WGSegmentationThreshold::widgetActivated);
	connect(buttonLeft   , &QAbstractButton::toggled, this, &WGSegmentationThreshold::widgetActivated);
	connect(buttonRight  , &QAbstractButton::toggled, this, &WGSegmentationThreshold::widgetActivated);

	connect(buttonAbsolut, &QAbstractButton::toggled, this, &WGSegmentationThreshold::widgetActivated);
	connect(buttonRelativ, &QAbstractButton::toggled, this, &WGSegmentationThreshold::widgetActivated);

}

void WGSegmentationThreshold::getThresholdData(BScanSegmentationMarker::ThresholdData& data)
{
	if(buttonUp && buttonUp   ->isChecked())
		data.direction = BScanSegmentationMarker::ThresholdData::Direction::up;
	else if(buttonLeft && buttonLeft ->isChecked())
		data.direction = BScanSegmentationMarker::ThresholdData::Direction::left;
	else if(buttonRight && buttonRight->isChecked())
		data.direction = BScanSegmentationMarker::ThresholdData::Direction::right;
	else
		data.direction = BScanSegmentationMarker::ThresholdData::Direction::down;

	if(buttonAbsolut && buttonAbsolut->isChecked())
		data.method = BScanSegmentationMarker::ThresholdData::Method::Absolute;
	else
		data.method = BScanSegmentationMarker::ThresholdData::Method::Relative;

	if(absoluteBox)
		data.absoluteValue = absoluteBox->value();
	if(relativeBox)
		data.relativeFrac  = relativeBox->value();
	if(strikesBox)
		data.neededStrikes = strikesBox ->value();
}

void WGSegmentationThreshold::absoluteSpinBoxChanged()
{
	if(buttonAbsolut)
		buttonAbsolut->setChecked(true);
}

void WGSegmentationThreshold::relativeSpinBoxChanged()
{
	if(buttonRelativ)
		buttonRelativ->setChecked(true);
}


void WGSegmentationThreshold::widgetActivated()
{
	emit(blockAction(true));
}


