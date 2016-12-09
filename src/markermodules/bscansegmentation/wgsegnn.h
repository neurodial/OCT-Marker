#ifndef WGSEGNN_H
#define WGSEGNN_H

#ifdef ML_SUPPORT

#include <QObject>

#include <ui_marker_segmentation_nn.h>

class BScanSegLocalOpNN;
class WGSegmentation;
class BScanSegmentation;

namespace BScanSegmentationMarker
{
	struct NNData;
}

class WgSegNN : public QWidget, Ui::BScanSegWidgetNN
{
	Q_OBJECT


	BScanSegmentation* segmentation = nullptr;
	BScanSegLocalOpNN* localOpNN    = nullptr;

	void createConnections();

	void setNNData(const BScanSegmentationMarker::NNData& data);
	void getNNData(      BScanSegmentationMarker::NNData& data);

public:
	WgSegNN(WGSegmentation* parent, BScanSegmentation* seg);
	virtual ~WgSegNN();


private slots:

	void slotLearnBScan();
	void slotTrain();

	void slotSave();
	void slotLoad();

	void slotLoadSaveButtonBoxClicked(QAbstractButton* button);

};

#endif

#endif // WGSEGNN_H
