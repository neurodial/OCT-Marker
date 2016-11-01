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
	thresSeries.setThresholdData(BScanSegmentationMarker::ThresholdData());



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
	thresBScan.setThresholdData(BScanSegmentationMarker::ThresholdData());


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
	thresLocal.setThresholdData(BScanSegmentationMarker::ThresholdData());


	localSizeSpinBox->setValue(segmentation->getLocalOperatorSize());

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
// 	connect(buttonBScanErode             , &QPushButton::pressed, this, &WGSegmentation::slotBscanErode          );
// 	connect(buttonBScanDilate            , &QPushButton::pressed, this, &WGSegmentation::slotBscanDilate         );
// 	connect(buttonBScanOpenClose         , &QPushButton::pressed, this, &WGSegmentation::slotBscanOpenClose      );
// 	connect(buttonBScanMedian            , &QPushButton::pressed, this, &WGSegmentation::slotBscanMedian         );

	connect(buttonBScanErode             , &QPushButton::pressed, segmentation, &BScanSegmentation::erodeBScan          );
	connect(buttonBScanDilate            , &QPushButton::pressed, segmentation, &BScanSegmentation::dilateBScan         );
	connect(buttonBScanOpenClose         , &QPushButton::pressed, segmentation, &BScanSegmentation::opencloseBScan      );
	connect(buttonBScanMedian            , &QPushButton::pressed, segmentation, &BScanSegmentation::medianBScan         );



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

	if(absoluteBox)
		connect(absoluteBox, static_cast<void(QSpinBox::*      )(int   )>(&QSpinBox::valueChanged)      , this, &WGSegmentationThreshold::absoluteSpinBoxChanged);
	if(relativeBox)
		connect(relativeBox, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &WGSegmentationThreshold::relativeSpinBoxChanged);

	if(absoluteBox)
		connect(absoluteBox, static_cast<void(QSpinBox::*      )(int   )>(&QSpinBox::valueChanged)      , this, &WGSegmentationThreshold::widgetActivated);
	if(relativeBox)
		connect(relativeBox, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &WGSegmentationThreshold::widgetActivated);
	if(strikesBox)
		connect(strikesBox , static_cast<void(QSpinBox::*      )(int   )>(&QSpinBox::valueChanged)      , this, &WGSegmentationThreshold::widgetActivated);

	if(buttonUp)
		connect(buttonUp     , &QAbstractButton::toggled, this, &WGSegmentationThreshold::widgetActivated);
	if(buttonDown)
		connect(buttonDown   , &QAbstractButton::toggled, this, &WGSegmentationThreshold::widgetActivated);
	if(buttonLeft)
		connect(buttonLeft   , &QAbstractButton::toggled, this, &WGSegmentationThreshold::widgetActivated);
	if(buttonRight)
		connect(buttonRight  , &QAbstractButton::toggled, this, &WGSegmentationThreshold::widgetActivated);

	if(buttonAbsolut)
		connect(buttonAbsolut, &QAbstractButton::toggled, this, &WGSegmentationThreshold::widgetActivated);
	if(buttonRelativ)
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

void WGSegmentationThreshold::setThresholdData(const BScanSegmentationMarker::ThresholdData& data)
{
	switch(data.direction)
	{
		case BScanSegmentationMarker::ThresholdData::Direction::up:
			if(buttonUp)
				buttonUp->setChecked(true);
			break;
		case BScanSegmentationMarker::ThresholdData::Direction::down:
			if(buttonDown)
				buttonDown->setChecked(true);
			break;
		case BScanSegmentationMarker::ThresholdData::Direction::left:
			if(buttonLeft)
				buttonLeft->setChecked(true);
			break;
		case BScanSegmentationMarker::ThresholdData::Direction::right:
			if(buttonRight)
				buttonRight->setChecked(true);
			break;
	}
	switch(data.method)
	{
		case BScanSegmentationMarker::ThresholdData::Method::Absolute:
			if(buttonAbsolut)
				buttonAbsolut->setChecked(true);
		case BScanSegmentationMarker::ThresholdData::Method::Relative:
			if(buttonRelativ)
				buttonRelativ->setChecked(true);
	}

	if(absoluteBox)
		absoluteBox->setValue(data.absoluteValue);
	if(relativeBox)
		relativeBox->setValue(data.relativeFrac );
	if(strikesBox)
		strikesBox ->setValue(data.neededStrikes);
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


