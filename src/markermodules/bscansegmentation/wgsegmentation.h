#ifndef WGSEGMENTATION_H
#define WGSEGMENTATION_H

#include <QWidget>

#include <ui_marker_segmentation.h>

class BScanSegmentation;

class BScanSegLocalOpPaint;
class BScanSegLocalOpThreshold;
class BScanSegLocalOpOperation;
class QButtonGroup;


namespace BScanSegmentationMarker { class ThresholdData; enum class LocalMethod; }

class WGSegmentation;

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
	void getThresholdData(      BScanSegmentationMarker::ThresholdData& data);
	void setThresholdData(const BScanSegmentationMarker::ThresholdData& data);

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

	BScanSegmentation* segmentation;

	WGSegmentationThreshold thresSeries;
	WGSegmentationThreshold thresBScan ;
	WGSegmentationThreshold thresLocal ;


	BScanSegLocalOpPaint*     localOpPaint     = nullptr;
	BScanSegLocalOpThreshold* localOpThreshold = nullptr;
	BScanSegLocalOpOperation* localOpOperation = nullptr;

	bool allowInitSeries = false;

	Orientation localThresholdOrientation = Orientation::Vertical;

	void createConnections();

	void setLocalThresholdOrientation(Orientation o);

	QButtonGroup* localMethodBG = nullptr;

public:
	WGSegmentation(BScanSegmentation* parent);
	virtual ~WGSegmentation();


private slots:
	void slotSeriesInitFromSeg();
	void slotSeriesInitFromThresh();

	void slotBscanInitFromThresh();

	void activateLocalPaint();
	void activateLocalThresh();
	void activateLocalOperation();

	void slotLocalThresh(bool);
	void slotLocalPaint(bool);
	void slotLocalOperation(bool);

	void switchSizeLocalThreshold();
	void switchSizeLocalOperation();

	void setCreateNewSeriesStartValueEnable(bool);
	void tabWidgetCurrentChanged(int index);

	void setLocalThresholdOrientationVertical()                     { setLocalThresholdOrientation(Orientation::Vertical  ); }
	void setLocalThresholdOrientationHorizontal()                   { setLocalThresholdOrientation(Orientation::Horizontal); }
	
	void setLocalOperator(BScanSegmentationMarker::LocalMethod method);
};

#endif // WGSEGMENTATION_H
