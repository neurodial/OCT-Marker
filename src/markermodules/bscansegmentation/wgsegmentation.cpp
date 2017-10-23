#include "wgsegmentation.h"

#include "bscansegmentation.h"
#include "configdata.h"
#include "bscanseglocalop.h"
#include "bscanseglocalopnn.h"

#include "wgsegnn.h"

#include <QButtonGroup>
#include <QFileDialog>


#include <octdata/datastruct/segmentationlines.h>
#include <manager/octdatamanager.h>

WGSegmentation::WGSegmentation(BScanSegmentation* parent)
: segmentation(parent)
, thresSeries(this)
, thresBScan (this)
, thresLocal (this)
{
	setupUi(this);

	tabWidget->setCurrentWidget(localTab);

	setCreateNewSeriesStartValueEnable(false);
	connect(checkBoxCreateNewSeriesStartValue, &QAbstractButton::toggled, this, &WGSegmentation::setCreateNewSeriesStartValueEnable);

	localOpPaint              = segmentation->getLocalOpPaint             ();
	localOpThresholdDirection = segmentation->getLocalOpThresholdDirection();
	localOpOperation          = segmentation->getLocalOpOperation         ();
	localOpThreshold          = segmentation->getLocalOpThreshold         ();

	setLocalOperator(segmentation->getLocalMethod());
	connect(segmentation, &BScanSegmentation::localOperatorChanged, this, &WGSegmentation::setLocalOperator);


	// Paint values:
	localSizePaintSpinBox->setValue(localOpPaint->getOperatorHeight());

	// Operator values
	localSizeOperationWidthSpinBox ->setValue(localOpOperation->getOperatorWidth());
	localSizeOperationHeightSpinBox->setValue(localOpOperation->getOperatorHeight());

	// Threshold values
	localSizeThresholdDirWidthSpinBox ->setValue(localOpThresholdDirection->getOperatorWidth());
	localSizeThresholdDirHeightSpinBox->setValue(localOpThresholdDirection->getOperatorHeight());


	// Button groups for series
	thresSeries.setButtonUp     (seriesThresholdDirectionUp     );
	thresSeries.setButtonDown   (seriesThresholdDirectionDown   );
//	thresSeries.setButtonLeft   ();
//	thresSeries.setButtonRight  ();

	thresSeries.setButtonAbsolut(seriesThresholdMethodAbs       );
	thresSeries.setButtonRelativ(seriesThresholdMethodRel       );

	thresSeries.setAbsoluteBox  (boxSeriesThresAbsolute         );
	thresSeries.setRelativeBox  (boxSeriesThresRelative         );
	thresSeries.setStrikesBox   (boxSeriesThresStrikes          );
	thresSeries.setStrFailFacBox(boxSeriesThresStrikesFailFactor);
	thresSeries.setupWidgets();
	thresSeries.setThresholdData(BScanSegmentationMarker::ThresholdDirectionData());



	// Button groups for BScan
	thresBScan.setButtonUp     (radioBScanThresFromBelow      );
	thresBScan.setButtonDown   (radioBScanThresFromAbove      );
//	thresBScan.setButtonLeft   ();
//	thresBScan.setButtonRight  ();

	thresBScan.setButtonAbsolut(radioBScanThresAbsolute       );
	thresBScan.setButtonRelativ(radioBScanThresRelative       );

	thresBScan.setAbsoluteBox  (boxBScanThresAbsolute         );
	thresBScan.setRelativeBox  (boxBScanThresRelative         );
	thresBScan.setStrikesBox   (boxBScanThresStrikes          );
	thresBScan.setStrFailFacBox(boxBScanThresStrikesFailFactor);
	thresBScan.setupWidgets();
	thresBScan.setThresholdData(BScanSegmentationMarker::ThresholdDirectionData());

	// Button groups for Local
	thresLocal.setButtonUp     (buttonLocalThresholdDirUp         );
	thresLocal.setButtonDown   (buttonLocalThresholdDirDown       );
	thresLocal.setButtonLeft   (buttonLocalThresholdDirLeft       );
	thresLocal.setButtonRight  (buttonLocalThresholdDirRight      );

	thresLocal.setButtonAbsolut(radioLocalThresholdDirAbsolut     );
	thresLocal.setButtonRelativ(radioLocalThresholdDirRelative    );

	thresLocal.setAbsoluteBox  (localThresholdDirAboluteSpinbox   );
	thresLocal.setRelativeBox  (localThresholdDirRelativeSpinBox  );
	thresLocal.setStrikesBox   (localThresholdDirStrikes          );
	thresLocal.setStrFailFacBox(localThresholdDirStrikesFailFactor);
	thresLocal.setupWidgets();
	thresLocal.setThresholdData(BScanSegmentationMarker::ThresholdDirectionData());


	localSizePaintSpinBox->setValue(localOpPaint->getOperatorHeight());
	checkBoxApplyOnMove->setChecked(localOpThresholdDirection->getApplyOnMouseMove());

	fillSeglineNames(comboBoxSeriesInitFromSeg);
	fillSeglineNames(comboBoxBScanFromSegline );

	// Button groups for local

	localMethodBG = new QButtonGroup(this);
	localMethodBG->addButton(radioLocalThreshold   );
	localMethodBG->addButton(radioLocalThresholdDir);
	localMethodBG->addButton(radioLocalPaint       );
	localMethodBG->addButton(radioLocalOperation   );
	localMethodBG->addButton(radioLocalNN          );

	QButtonGroup* localPaintMethodBG = new QButtonGroup(this);
	localPaintMethodBG->addButton(buttonLocalPaintCircle);
	localPaintMethodBG->addButton(buttonLocalPaintRec   );
	localPaintMethodBG->addButton(buttonLocalPaintPen   );
	buttonLocalPaintCircle->setChecked(localOpPaint->getPaintData().paintMethod == BScanSegmentationMarker::PaintData::PaintMethod::Circle);
	buttonLocalPaintRec   ->setChecked(localOpPaint->getPaintData().paintMethod == BScanSegmentationMarker::PaintData::PaintMethod::Rect  );
	buttonLocalPaintPen   ->setChecked(localOpPaint->getPaintData().paintMethod == BScanSegmentationMarker::PaintData::PaintMethod::Pen  );
	connect(localPaintMethodBG, static_cast<void(QButtonGroup::*)(int)>(&QButtonGroup::buttonClicked), this, &WGSegmentation::activateLocalPaint);

	QButtonGroup* localPaintAreaBG = new QButtonGroup(this);
	localPaintAreaBG->addButton(buttonLocalPaintArea0   );
	localPaintAreaBG->addButton(buttonLocalPaintAreaAuto);
	localPaintAreaBG->addButton(buttonLocalPaintArea1   );
	buttonLocalPaintArea0   ->setChecked(localOpPaint->getPaintData().paintColor == BScanSegmentationMarker::PaintData::PaintColor::Area0);
	buttonLocalPaintAreaAuto->setChecked(localOpPaint->getPaintData().paintColor == BScanSegmentationMarker::PaintData::PaintColor::Auto );
	buttonLocalPaintArea1   ->setChecked(localOpPaint->getPaintData().paintColor == BScanSegmentationMarker::PaintData::PaintColor::Area1);
	connect(localPaintAreaBG, static_cast<void(QButtonGroup::*)(int)>(&QButtonGroup::buttonClicked), this, &WGSegmentation::activateLocalPaint);

	QButtonGroup* localOperationBG = new QButtonGroup(this);
	localOperationBG->addButton(buttonLocalOperationErode    );
	localOperationBG->addButton(buttonLocalOperationDilate   );
	localOperationBG->addButton(buttonLocalOperationOpenClose);
	localOperationBG->addButton(buttonLocalOperationMedian   );
	buttonLocalOperationErode    ->setChecked(localOpOperation->getOperationData() == BScanSegmentationMarker::Operation::Erode    );
	buttonLocalOperationDilate   ->setChecked(localOpOperation->getOperationData() == BScanSegmentationMarker::Operation::Dilate   );
	buttonLocalOperationOpenClose->setChecked(localOpOperation->getOperationData() == BScanSegmentationMarker::Operation::OpenClose);
	buttonLocalOperationMedian   ->setChecked(localOpOperation->getOperationData() == BScanSegmentationMarker::Operation::Median   );
	connect(localOperationBG               , static_cast<void(QButtonGroup::*)(int)>(&QButtonGroup::buttonClicked), this, &WGSegmentation::activateLocalOperation);


	QButtonGroup* localThresholdBG = new QButtonGroup(this);
	localThresholdBG->addButton(radioLocalThresholdAbsolut );
	localThresholdBG->addButton(radioLocalThresholdRelative);
	setLocalThresholdValues();
	connect(localThresholdBG               , static_cast<void(QButtonGroup::*)(int)>(&QButtonGroup::buttonClicked), this, &WGSegmentation::activateLocalThresh);





	connect(localSizePaintSpinBox             , static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged)               , this, &WGSegmentation::activateLocalPaint    );

	connect(localSizeThresholdDirWidthSpinBox , static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged)               , this, &WGSegmentation::activateLocalThreshDir);
	connect(localSizeThresholdDirHeightSpinBox, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged)               , this, &WGSegmentation::activateLocalThreshDir);
	connect(checkBoxApplyOnMove               , &QCheckBox::toggled                                                        , this, &WGSegmentation::activateLocalThreshDir);

	connect(localSizeOperationWidthSpinBox    , static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged)               , this, &WGSegmentation::activateLocalOperation);
	connect(localSizeOperationHeightSpinBox   , static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged)               , this, &WGSegmentation::activateLocalOperation);


	connect(localSizeThresholdWidthSpinBox    , static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged)               , this, &WGSegmentation::activateLocalThresh   );
	connect(localSizeThresholdHeightSpinBox   , static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged)               , this, &WGSegmentation::activateLocalThresh   );
	connect(localThresholdAboluteSpinbox      , static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged)               , this, &WGSegmentation::activateLocalThreshAbs);
	connect(localThresholdRelativeSpinBox     , static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &WGSegmentation::activateLocalThreshRel);


	connect(switchSizeLocalThresholdDirButton, &QAbstractButton::clicked, this, &WGSegmentation::switchSizeLocalThresholdDir);
	connect(switchSizeLocalOperationButton   , &QAbstractButton::clicked, this, &WGSegmentation::switchSizeLocalOperation   );
	connect(switchSizeLocalThresholdButton   , &QAbstractButton::clicked, this, &WGSegmentation::switchSizeLocalThreshold   );


	// Icons
	buttonLocalPaintArea0   ->setIcon(localOpPaint->getPaintColorIcon(BScanSegmentationMarker::PaintData::PaintColor::Area0));
	buttonLocalPaintAreaAuto->setIcon(localOpPaint->getPaintColorIcon(BScanSegmentationMarker::PaintData::PaintColor::Auto ));
	buttonLocalPaintArea1   ->setIcon(localOpPaint->getPaintColorIcon(BScanSegmentationMarker::PaintData::PaintColor::Area1));

	connect(tabWidget, &QTabWidget::currentChanged, this, &WGSegmentation::tabWidgetCurrentChanged);

	createConnections();

#ifdef ML_SUPPORT
	widgetNN = new WgSegNN(this, parent);
	tabWidget->addTab(widgetNN, "NN");

#else
	line_nn->setVisible(false);
	radioLocalNN->setVisible(false);
#endif
}


WGSegmentation::~WGSegmentation()
{
}

void WGSegmentation::fillSeglineNames(QComboBox* combobox)
{
	if(!combobox)
		return;
	for(OctData::Segmentationlines::SegmentlineType type : OctData::Segmentationlines::getSegmentlineTypes())
		combobox->addItem(OctData::Segmentationlines::getSegmentlineName(type));
}

void WGSegmentation::initBScanFromSegline()
{
	std::size_t index = static_cast<std::size_t>(comboBoxBScanFromSegline->currentIndex());
	if(index < OctData::Segmentationlines::getSegmentlineTypes().size())
		segmentation->initBScanFromSegline(static_cast<OctData::Segmentationlines::SegmentlineType>(index));
}

void WGSegmentation::initSeriesFromSegline()
{
	std::size_t index = static_cast<std::size_t>(comboBoxSeriesInitFromSeg->currentIndex());
	if(index < OctData::Segmentationlines::getSegmentlineTypes().size())
		segmentation->initSeriesFromSegline(static_cast<OctData::Segmentationlines::SegmentlineType>(index));
}



void WGSegmentation::createConnections()
{
	connect(buttonSeriesInitFromThreshold, &QPushButton::clicked, this, &WGSegmentation::slotSeriesInitFromThresh);

	connect(buttonBScanInitFromThreshold , &QPushButton::clicked, this, &WGSegmentation::slotBscanInitFromThresh );
	connect(buttonImportOCT              , &QPushButton::clicked, this, &WGSegmentation::importSegmentationFromOctSlot );


	connect(buttonBScanErode             , &QPushButton::clicked, segmentation, &BScanSegmentation::erodeBScan          );
	connect(buttonBScanDilate            , &QPushButton::clicked, segmentation, &BScanSegmentation::dilateBScan         );
	connect(buttonBScanOpenClose         , &QPushButton::clicked, segmentation, &BScanSegmentation::opencloseBScan      );
	connect(buttonBScanMedian            , &QPushButton::clicked, segmentation, &BScanSegmentation::medianBScan         );

	// set local threshold
	connect(&thresLocal       , &WGSegmentationThreshold::blockAction, this, &WGSegmentation::activateLocalThreshDir);

	connect(radioLocalThreshold      , &QRadioButton::toggled   , this, &WGSegmentation::slotLocalThresh                       );
	connect(radioLocalThresholdDir   , &QRadioButton::toggled   , this, &WGSegmentation::slotLocalThreshDir                    );
	connect(radioLocalPaint          , &QRadioButton::toggled   , this, &WGSegmentation::slotLocalPaint                        );
	connect(radioLocalOperation      , &QRadioButton::toggled   , this, &WGSegmentation::slotLocalOperation                    );
	connect(radioLocalNN             , &QRadioButton::toggled   , this, &WGSegmentation::slotLocalNN                           );

	connect(buttonLocalThresholdDirUp   , &QAbstractButton::clicked, this, &WGSegmentation::setLocalThresholdOrientationVertical  );
	connect(buttonLocalThresholdDirDown , &QAbstractButton::clicked, this, &WGSegmentation::setLocalThresholdOrientationVertical  );
	connect(buttonLocalThresholdDirLeft , &QAbstractButton::clicked, this, &WGSegmentation::setLocalThresholdOrientationHorizontal);
	connect(buttonLocalThresholdDirRight, &QAbstractButton::clicked, this, &WGSegmentation::setLocalThresholdOrientationHorizontal);

	connect(buttonBScanInitFromSeg        , &QAbstractButton::clicked, this, &WGSegmentation::initBScanFromSegline    );
	connect(buttonSeriesInitFromSeg       , &QAbstractButton::clicked, this, &WGSegmentation::initSeriesFromSegline   );
	connect(buttonSeriesDeleteSegmentation, &QAbstractButton::clicked, segmentation, &BScanSegmentation::removeSeriesSegmentation);


	connect(buttonSeriesExtendLeftRightSpace , &QAbstractButton::clicked, segmentation, &BScanSegmentation::seriesExtendLeftRightSpace);
	connect(buttonSeriesRemoveUnconectedAreas, &QAbstractButton::clicked, segmentation, &BScanSegmentation::seriesRemoveUnconectedAreas);

	connect(buttonBScanExtendLeftRightSpace , &QAbstractButton::clicked, segmentation, &BScanSegmentation::extendLeftRightSpace);
	connect(buttonBScanRemoveUnconectedAreas, &QAbstractButton::clicked, segmentation, &BScanSegmentation::removeUnconectedAreas);
}



void WGSegmentation::slotSeriesInitFromThresh()
{
	if(!allowInitSeries)
		return;
	BScanSegmentationMarker::ThresholdDirectionData data;
	thresSeries.getThresholdData(data);
	segmentation->initSeriesFromThreshold(data);
}




void WGSegmentation::slotBscanInitFromThresh()
{
	BScanSegmentationMarker::ThresholdDirectionData data;
	thresBScan.getThresholdData(data);
	segmentation->initBScanFromThreshold(data);
}


void WGSegmentation::slotLocalThresh(bool checked)
{
	if(!checked)
		return;

	BScanSegmentationMarker::ThresholdData data;
	data.absoluteValue = static_cast<BScanSegmentationMarker::internalMatType>(localThresholdAboluteSpinbox->value());
	data.relativeFrac  = localThresholdRelativeSpinBox->value();


	if(radioLocalThresholdAbsolut->isChecked())
		data.method = BScanSegmentationMarker::ThresholdMethod::Absolute;
	else
		data.method = BScanSegmentationMarker::ThresholdMethod::Relative;

	localOpThreshold->setThresholdData(data);

	localOpThreshold->setOperatorSizeWidth (localSizeThresholdWidthSpinBox ->value());
	localOpThreshold->setOperatorSizeHeight(localSizeThresholdHeightSpinBox->value());

	segmentation->setLocalMethod(BScanSegmentationMarker::LocalMethod::Threshold);
}



void WGSegmentation::slotLocalThreshDir(bool checked)
{
	if(!checked)
		return;

	BScanSegmentationMarker::ThresholdDirectionData data;
	thresLocal.getThresholdData(data);

	localOpThresholdDirection->setThresholdData(data);
	localOpThresholdDirection->setOperatorSizeWidth (localSizeThresholdDirWidthSpinBox ->value());
	localOpThresholdDirection->setOperatorSizeHeight(localSizeThresholdDirHeightSpinBox->value());
	localOpThresholdDirection->setApplyOnMouseMove(checkBoxApplyOnMove->isChecked());

	segmentation->setLocalMethod(BScanSegmentationMarker::LocalMethod::ThresholdDirection);
}

void WGSegmentation::slotLocalPaint(bool checked)
{
	if(!checked)
		return;

	BScanSegmentationMarker::PaintData data;

	if(buttonLocalPaintArea0->isChecked())
		data.paintColor = BScanSegmentationMarker::PaintData::PaintColor::Area0;
	else if(buttonLocalPaintArea1->isChecked())
		data.paintColor = BScanSegmentationMarker::PaintData::PaintColor::Area1;
	else
		data.paintColor = BScanSegmentationMarker::PaintData::PaintColor::Auto;

	if(buttonLocalPaintCircle->isChecked())
		data.paintMethod = BScanSegmentationMarker::PaintData::PaintMethod::Circle;
	else if(buttonLocalPaintRec->isChecked())
		data.paintMethod = BScanSegmentationMarker::PaintData::PaintMethod::Rect;
	else
		data.paintMethod = BScanSegmentationMarker::PaintData::PaintMethod::Pen;

	localOpPaint->setPaintData(data);
	localOpPaint->setOperatorSize(localSizePaintSpinBox->value());
	segmentation->setLocalMethod(BScanSegmentationMarker::LocalMethod::Paint);
}

void WGSegmentation::slotLocalOperation(bool checked)
{
	if(!checked)
		return;

	     if(buttonLocalOperationErode    ->isChecked()) localOpOperation->setOperationData(BScanSegmentationMarker::Operation::Erode    );
	else if(buttonLocalOperationDilate   ->isChecked()) localOpOperation->setOperationData(BScanSegmentationMarker::Operation::Dilate   );
	else if(buttonLocalOperationOpenClose->isChecked()) localOpOperation->setOperationData(BScanSegmentationMarker::Operation::OpenClose);
	else                                                localOpOperation->setOperationData(BScanSegmentationMarker::Operation::Median   );

	localOpOperation->setOperatorSizeWidth (localSizeOperationWidthSpinBox ->value());
	localOpOperation->setOperatorSizeHeight(localSizeOperationHeightSpinBox->value());

	segmentation->setLocalMethod(BScanSegmentationMarker::LocalMethod::Operation);
}

void WGSegmentation::slotLocalNN(bool checked)
{
	if(!checked)
		return;
	segmentation->setLocalMethod(BScanSegmentationMarker::LocalMethod::NN);
}



void WGSegmentation::activateLocalThresh()
{
	if(radioLocalThreshold->isChecked())
		slotLocalThresh(true);
	else
		radioLocalThreshold->setChecked(true);
}

void WGSegmentation::activateLocalThreshAbs()
{
	if(radioLocalThresholdAbsolut->isChecked())
		slotLocalThresh(true);
	else
	{
		radioLocalThresholdAbsolut->setChecked(true);
		activateLocalThresh();
	}
}

void WGSegmentation::activateLocalThreshRel()
{
	if(radioLocalThresholdRelative->isChecked())
		slotLocalThresh(true);
	else
	{
		radioLocalThresholdRelative->setChecked(true);
		activateLocalThresh();
	}
}

void WGSegmentation::activateLocalThreshDir()
{
	if(radioLocalThresholdDir->isChecked())
		slotLocalThreshDir(true);
	else
		radioLocalThresholdDir->setChecked(true);
}

void WGSegmentation::activateLocalPaint()
{
	if(radioLocalPaint->isChecked())
		slotLocalPaint(true);
	else
		radioLocalPaint->setChecked(true);
}

void WGSegmentation::activateLocalOperation()
{
	if(radioLocalOperation->isChecked())
		slotLocalOperation(true);
	else
		radioLocalOperation->setChecked(true);
}

void WGSegmentation::activateLocalNN()
{
	if(radioLocalNN->isChecked())
		slotLocalNN(true);
	else
		radioLocalNN->setChecked(true);
}



void WGSegmentation::switchSizeLocalOperation()
{
	int val1 = localSizeOperationWidthSpinBox ->value();
	int val2 = localSizeOperationHeightSpinBox->value();

	localSizeOperationWidthSpinBox ->setValue(val2);
	localSizeOperationHeightSpinBox->setValue(val1);
}

void WGSegmentation::switchSizeLocalThresholdDir()
{
	int val1 = localSizeThresholdDirWidthSpinBox ->value();
	int val2 = localSizeThresholdDirHeightSpinBox->value();

	localSizeThresholdDirWidthSpinBox ->setValue(val2);
	localSizeThresholdDirHeightSpinBox->setValue(val1);
}

void WGSegmentation::switchSizeLocalThreshold()
{
	int val1 = localSizeThresholdWidthSpinBox ->value();
	int val2 = localSizeThresholdHeightSpinBox->value();

	localSizeThresholdWidthSpinBox ->setValue(val2);
	localSizeThresholdHeightSpinBox->setValue(val1);
}

void WGSegmentation::tabWidgetCurrentChanged(int /*index*/)
{
	if(allowInitSeries)
		setCreateNewSeriesStartValueEnable(false);
}


void WGSegmentation::setCreateNewSeriesStartValueEnable(bool b)
{
	buttonSeriesInitFromThreshold ->setEnabled(b);
	buttonSeriesInitFromSeg       ->setEnabled(b);
	buttonSeriesDeleteSegmentation->setEnabled(b);

	buttonSeriesExtendLeftRightSpace ->setEnabled(b);
	buttonSeriesRemoveUnconectedAreas->setEnabled(b);

	buttonImportOCT                  ->setEnabled(b);

	checkBoxCreateNewSeriesStartValue->setChecked(b);


	allowInitSeries = b;
}

void WGSegmentation::setLocalThresholdDirOrientation(Orientation o)
{
	if(o != localThresholdDirOrientation)
	{
		if(autoSwitchSizeLocalThresholdDirButton->isChecked())
			switchSizeLocalThresholdDir();
		localThresholdDirOrientation = o;
	}
}


void WGSegmentation::setLocalOperator(BScanSegmentationMarker::LocalMethod method)
{
	switch(method)
	{
		case BScanSegmentationMarker::LocalMethod::Paint:
			radioLocalPaint->setChecked(true);
			break;
		case BScanSegmentationMarker::LocalMethod::ThresholdDirection:
			radioLocalThresholdDir->setChecked(true);
			break;
		case BScanSegmentationMarker::LocalMethod::Operation:
			radioLocalOperation->setChecked(true);
			break;
		case BScanSegmentationMarker::LocalMethod::Threshold:
			radioLocalThreshold->setChecked(true);
			break;
		case BScanSegmentationMarker::LocalMethod::None:
		{
			QAbstractButton* checkedButton = localMethodBG->checkedButton();
			if(checkedButton)
			{
				localMethodBG->setExclusive(false);
				checkedButton->setChecked(false);
				localMethodBG->setExclusive(true);
			}
			break;
		}
		case BScanSegmentationMarker::LocalMethod::NN:
			radioLocalNN->setChecked(true);
			break;
	}
}

void WGSegmentation::setLocalThresholdValues()
{
	const BScanSegmentationMarker::ThresholdData& data = localOpThreshold->getLocalThresholdData();

	localSizeThresholdWidthSpinBox ->setValue(localOpThreshold->getOperatorSizeWidth() );
	localSizeThresholdHeightSpinBox->setValue(localOpThreshold->getOperatorSizeHeight());

	localThresholdAboluteSpinbox ->setValue(data.absoluteValue);
	localThresholdRelativeSpinBox->setValue(data.relativeFrac );

	radioLocalThresholdAbsolut ->setChecked(data.method == BScanSegmentationMarker::ThresholdMethod::Absolute);
	radioLocalThresholdRelative->setChecked(data.method == BScanSegmentationMarker::ThresholdMethod::Relative);

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
	{
		connect(absoluteBox, static_cast<void(QSpinBox::*      )(int   )>(&QSpinBox::valueChanged)      , this, &WGSegmentationThreshold::absoluteSpinBoxChanged);
		absoluteBox->setSingleStep(5);
		absoluteBox->setMinimum(0);
		absoluteBox->setMaximum(255);
	}
	if(relativeBox)
	{
		connect(relativeBox, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &WGSegmentationThreshold::relativeSpinBoxChanged);
		relativeBox->setSingleStep(0.05);
		relativeBox->setMinimum(0);
		relativeBox->setMaximum(1);
	}
	if(strFailFacBox)
	{
		strFailFacBox->setSingleStep(0.05);
		strFailFacBox->setMinimum(0);
		strFailFacBox->setMaximum(1);
	}

	if(absoluteBox  ) connect(absoluteBox  , static_cast<void(QSpinBox::*      )(int   )>(&QSpinBox::valueChanged)      , this, &WGSegmentationThreshold::widgetActivated);
	if(relativeBox  ) connect(relativeBox  , static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &WGSegmentationThreshold::widgetActivated);
	if(strikesBox   ) connect(strikesBox   , static_cast<void(QSpinBox::*      )(int   )>(&QSpinBox::valueChanged)      , this, &WGSegmentationThreshold::widgetActivated);
	if(strFailFacBox) connect(strFailFacBox, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &WGSegmentationThreshold::widgetActivated);

	if(buttonUp     ) connect(buttonUp     , &QAbstractButton::clicked, this, &WGSegmentationThreshold::widgetActivated);
	if(buttonDown   ) connect(buttonDown   , &QAbstractButton::clicked, this, &WGSegmentationThreshold::widgetActivated);
	if(buttonLeft   ) connect(buttonLeft   , &QAbstractButton::clicked, this, &WGSegmentationThreshold::widgetActivated);
	if(buttonRight  ) connect(buttonRight  , &QAbstractButton::clicked, this, &WGSegmentationThreshold::widgetActivated);

	if(buttonAbsolut) connect(buttonAbsolut, &QAbstractButton::clicked, this, &WGSegmentationThreshold::widgetActivated);
	if(buttonRelativ) connect(buttonRelativ, &QAbstractButton::clicked, this, &WGSegmentationThreshold::widgetActivated);

}

void WGSegmentationThreshold::getThresholdData(BScanSegmentationMarker::ThresholdDirectionData& data)
{
	if(buttonUp && buttonUp   ->isChecked())
		data.direction = BScanSegmentationMarker::ThresholdDirectionData::Direction::up;
	else if(buttonLeft && buttonLeft ->isChecked())
		data.direction = BScanSegmentationMarker::ThresholdDirectionData::Direction::left;
	else if(buttonRight && buttonRight->isChecked())
		data.direction = BScanSegmentationMarker::ThresholdDirectionData::Direction::right;
	else
		data.direction = BScanSegmentationMarker::ThresholdDirectionData::Direction::down;

	if(buttonAbsolut && buttonAbsolut->isChecked())
		data.method = BScanSegmentationMarker::ThresholdMethod::Absolute;
	else
		data.method = BScanSegmentationMarker::ThresholdMethod::Relative;

	if(absoluteBox)
		data.absoluteValue = static_cast<BScanSegmentationMarker::internalMatType>(absoluteBox->value());
	if(relativeBox)
		data.relativeFrac  = relativeBox->value();
	if(strikesBox)
		data.neededStrikes = static_cast<int>(strikesBox->value());
	if(strFailFacBox)
		data.negStrikesFactor = strFailFacBox->value();
}

void WGSegmentationThreshold::setThresholdData(const BScanSegmentationMarker::ThresholdDirectionData& data)
{
	switch(data.direction)
	{
		case BScanSegmentationMarker::ThresholdDirectionData::Direction::up:
			if(buttonUp)
				buttonUp->setChecked(true);
			break;
		case BScanSegmentationMarker::ThresholdDirectionData::Direction::down:
			if(buttonDown)
				buttonDown->setChecked(true);
			break;
		case BScanSegmentationMarker::ThresholdDirectionData::Direction::left:
			if(buttonLeft)
				buttonLeft->setChecked(true);
			break;
		case BScanSegmentationMarker::ThresholdDirectionData::Direction::right:
			if(buttonRight)
				buttonRight->setChecked(true);
			break;
	}

	if(absoluteBox)
		absoluteBox->setValue(data.absoluteValue);
	if(relativeBox)
		relativeBox->setValue(data.relativeFrac );
	if(strikesBox)
		strikesBox ->setValue(static_cast<int>(data.neededStrikes));
	if(strFailFacBox)
		strFailFacBox->setValue(data.negStrikesFactor);

	switch(data.method)
	{
		case BScanSegmentationMarker::ThresholdMethod::Absolute:
			if(buttonAbsolut)
				buttonAbsolut->setChecked(true);
		case BScanSegmentationMarker::ThresholdMethod::Relative:
			if(buttonRelativ)
				buttonRelativ->setChecked(true);
	}
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
	emit(blockAction());
}


void WGSegmentation::importSegmentationFromOctSlot()
{
// 		EYE00021_E_1776.vol_seg_bin.bin
	const QString& filename = OctDataManager::getInstance().getLoadedFilename();
	QString importfile = filename + "_seg_bin.bin";
	QFileInfo fileinfo(importfile);

	if(!fileinfo.exists())
		importfile = QString();

	QString file = QFileDialog::getOpenFileName(this, tr("Import marker from oct file"), importfile, ".bin");
	if(!file.isEmpty())
	{
		segmentation->importSegmentationFromOct(file.toStdString());
	}
}


