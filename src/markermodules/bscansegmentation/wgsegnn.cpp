#include "wgsegnn.h"

#ifdef ML_SUPPORT

#include "wgsegmentation.h"
#include "bscansegmentation.h"
#include "bscanseglocalopnn.h"

#include <helper/callback.h>

#include <QFileDialog>


WgSegNN::WgSegNN(WGSegmentation* parent, BScanSegmentation* seg)
: QWidget(parent)
, segmentation(seg)
, localOpNN(seg->getLocalOpNN())
{
	setupUi(this);

	setNNData(BScanSegmentationMarker::NNTrainData());

	labelNumberExampels->setText("0");

	createConnections();
}


WgSegNN::~WgSegNN()
{
}


void WgSegNN::createConnections()
{

	connect(buttonBoxLoadSave , &QDialogButtonBox::clicked, this, &WgSegNN::slotLoadSaveButtonBoxClicked);
	connect(pushButtonTrain   , &QAbstractButton ::clicked, this, &WgSegNN::slotTrain                   );
	connect(pbAddBscanExampels, &QAbstractButton ::clicked, this, &WgSegNN::slotAddBscanExampels        );

}


void WgSegNN::slotLoadSaveButtonBoxClicked(QAbstractButton* button)
{
	QDialogButtonBox::StandardButton stdButton = buttonBoxLoadSave->standardButton(button);

	switch(stdButton)
	{
		case QDialogButtonBox::Open:
			slotLoad();
			break;
		case QDialogButtonBox::Save:
			slotSave();
			break;
		default:
			break;
	}
}


void WgSegNN::getNNData(BScanSegmentationMarker::NNTrainData& data)
{
	data.maxIterations = sbMaxIterations->value();
	data.epsilon       = sbEpsilon      ->value();

	data.learnRate     = sbLearnRate    ->value();
	data.momentScale   = sbMomentScale  ->value();

	data.delta0        = sbDelta0       ->value();
	data.deltaMax      = sbDeltaMax     ->value();
	data.deltaMin      = sbDeltaMin     ->value();
	data.nueMinus      = sbNueMinus     ->value();
	data.nuePlus       = sbNuePlus      ->value();

	if(rbBackPropagation->isChecked())
		data.trainMethod = BScanSegmentationMarker::NNTrainData::TrainMethod::Backpropergation;
	else
		data.trainMethod = BScanSegmentationMarker::NNTrainData::TrainMethod::RPROP           ;
}


void WgSegNN::setNNData(const BScanSegmentationMarker::NNTrainData& data)
{

	sbMaxIterations->setValue(static_cast<int>(data.maxIterations));
	sbEpsilon      ->setValue(data.epsilon      );

	sbLearnRate    ->setValue(data.learnRate    );
	sbMomentScale  ->setValue(data.momentScale  );

	sbDelta0       ->setValue(data.delta0       );
	sbDeltaMax     ->setValue(data.deltaMax     );
	sbDeltaMin     ->setValue(data.deltaMin     );
	sbNueMinus     ->setValue(data.nueMinus     );
	sbNuePlus      ->setValue(data.nuePlus      );

	rbBackPropagation->setChecked(data.trainMethod == BScanSegmentationMarker::NNTrainData::TrainMethod::Backpropergation);
	rbRPROP          ->setChecked(data.trainMethod == BScanSegmentationMarker::NNTrainData::TrainMethod::RPROP           );
}



void WgSegNN::slotTrain()
{
//	CallbackProgressDialog process("Learn BScan", "Cancel");
	BScanSegmentationMarker::NNTrainData data;
	getNNData(data);
	localOpNN->trainNN(data);
}

void WgSegNN::slotLoad()
{
	QString file = QFileDialog::getOpenFileName(this, tr("Load NN"), QString(), "*.yml");
	if(!file.isEmpty())
		localOpNN->loadNN(file);
}

void WgSegNN::slotSave()
{
	QString file = QFileDialog::getSaveFileName(this, tr("Save NN"), QString(), "*.yml");
	if(!file.isEmpty())
		localOpNN->saveNN(file);
}

void WgSegNN::slotAddBscanExampels()
{
	localOpNN->addBscanExampels();
	labelNumberExampels->setText(QString("%1").arg(localOpNN->numExampels()));
}



#endif
