/*
 * Copyright (c) 2018 Kay Gawlik <kaydev@amarunet.de> <kay.gawlik@beuth-hochschule.de> <kay.gawlik@charite.de>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef WGSEGNN_H
#define WGSEGNN_H

#ifdef ML_SUPPORT

#include <QObject>

#include <ui_marker_segmentation_nn.h>

class BScanSegLocalOpNN;
class WGSegmentation;
class BScanSegmentation;
class WindowNNInOut;

namespace BScanSegmentationMarker
{
	struct NNTrainData;
}

class WgSegNN : public QWidget, Ui::BScanSegWidgetNN
{
	Q_OBJECT


	BScanSegmentation* segmentation = nullptr;
	BScanSegLocalOpNN* localOpNN    = nullptr;

	WindowNNInOut* inOutWindow = nullptr;

	void createConnections();

	void setNNData(const BScanSegmentationMarker::NNTrainData& data);
	void getNNData(      BScanSegmentationMarker::NNTrainData& data);

	void updateActLayerInfo();
	void updateExampleInfo();

public:
	WgSegNN(WGSegmentation* parent, BScanSegmentation* seg);
	virtual ~WgSegNN();


private slots:

	// void slotLearnBScan();
	void slotTrain();

	void slotAddBscanExampels();

	void slotSave();
	void slotLoad();

	void slotLoadSaveButtonBoxClicked(QAbstractButton* button);

	void changeNNConfig();

	void showInOutWindow(bool show);

};

#endif

#endif // WGSEGNN_H
