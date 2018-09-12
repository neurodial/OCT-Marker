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

#ifndef WGSEGMENTATION_H
#define WGSEGMENTATION_H

#include <QWidget>

#include <ui_marker_segmentation.h>

class BScanSegmentation;

class BScanSegLocalOpPaint;
class BScanSegLocalOpThreshold;
class BScanSegLocalOpThresholdDirection;
class BScanSegLocalOpOperation;
class BScanSegLocalOpNN;
class QButtonGroup;


namespace BScanSegmentationMarker { class ThresholdDirectionData; enum class LocalMethod; }

class WGSegmentation;
class WgSegNN;

class WGSegmentationThreshold : public QObject
{
	Q_OBJECT

	WGSegmentation* widget         = nullptr;
	QAbstractButton* buttonUp      = nullptr;
	QAbstractButton* buttonDown    = nullptr;
	QAbstractButton* buttonLeft    = nullptr;
	QAbstractButton* buttonRight   = nullptr;

	QAbstractButton* buttonAbsolut = nullptr;
	QAbstractButton* buttonRelativ = nullptr;

	QSpinBox*        absoluteBox   = nullptr;
	QDoubleSpinBox*  relativeBox   = nullptr;
	QSpinBox*        strikesBox    = nullptr;
	QDoubleSpinBox*  strFailFacBox = nullptr;

public:
	WGSegmentationThreshold(WGSegmentation* parent);

	void setButtonUp     (QAbstractButton* item)                   { buttonUp      = item; }
	void setButtonDown   (QAbstractButton* item)                   { buttonDown    = item; }
	void setButtonLeft   (QAbstractButton* item)                   { buttonLeft    = item; }
	void setButtonRight  (QAbstractButton* item)                   { buttonRight   = item; }

	void setButtonAbsolut(QAbstractButton* item)                   { buttonAbsolut = item; }
	void setButtonRelativ(QAbstractButton* item)                   { buttonRelativ = item; }

	void setAbsoluteBox  (QSpinBox*        item)                   { absoluteBox   = item; }
	void setRelativeBox  (QDoubleSpinBox*  item)                   { relativeBox   = item; }
	void setStrikesBox   (QSpinBox*        item)                   { strikesBox    = item; }
	void setStrFailFacBox(QDoubleSpinBox*  item)                   { strFailFacBox = item; }

	void setupWidgets();
	void getThresholdData(      BScanSegmentationMarker::ThresholdDirectionData& data);
	void setThresholdData(const BScanSegmentationMarker::ThresholdDirectionData& data);

private slots:
	void absoluteSpinBoxChanged();
	void relativeSpinBoxChanged();
	void widgetActivated();

signals:
	void blockAction();
};


class WGSegmentation : public QWidget, Ui::BScanSegmentationWidget
{
	Q_OBJECT;
	enum class Orientation { Vertical, Horizontal };

	BScanSegmentation* segmentation = nullptr;

	WGSegmentationThreshold thresSeries;
	WGSegmentationThreshold thresBScan ;
	WGSegmentationThreshold thresLocal ;


	BScanSegLocalOpPaint*              localOpPaint              = nullptr;
	BScanSegLocalOpThresholdDirection* localOpThresholdDirection = nullptr;
	BScanSegLocalOpThreshold*          localOpThreshold          = nullptr;
	BScanSegLocalOpOperation*          localOpOperation          = nullptr;

	WgSegNN* widgetNN = nullptr;


	bool allowInitSeries = false;

	Orientation localThresholdDirOrientation = Orientation::Vertical;

	void createConnections();

	void setLocalThresholdDirOrientation(Orientation o);

	void setLocalThresholdValues();

	QButtonGroup* localMethodBG = nullptr;

	void fillSeglineNames(QComboBox* combobox);

public:
	WGSegmentation(BScanSegmentation* parent);
	virtual ~WGSegmentation();


private slots:
	void slotSeriesInitFromThresh();

	void slotBscanInitFromThresh();

	void activateLocalThresh();
	void activateLocalThreshRel();
	void activateLocalThreshAbs();
	void activateLocalThreshDir();
	void activateLocalPaint();
	void activateLocalOperation();
	void activateLocalNN();

	void slotLocalThresh(bool);
	void slotLocalThreshDir(bool);
	void slotLocalPaint(bool);
	void slotLocalOperation(bool);
	void slotLocalNN(bool);

	void setPaintColor();

	void switchSizeLocalThresholdDir();
	void switchSizeLocalThreshold();
	void switchSizeLocalOperation();

	void setCreateNewSeriesStartValueEnable(bool);
	void tabWidgetCurrentChanged(int index);

	void setLocalThresholdOrientationVertical()                     { setLocalThresholdDirOrientation(Orientation::Vertical  ); }
	void setLocalThresholdOrientationHorizontal()                   { setLocalThresholdDirOrientation(Orientation::Horizontal); }
	
	void setLocalOperator(BScanSegmentationMarker::LocalMethod method);


	void initSeriesFromSegline();
	void initBScanFromSegline();

};

#endif // WGSEGMENTATION_H
