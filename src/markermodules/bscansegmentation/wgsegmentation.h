#ifndef WGSEGMENTATION_H
#define WGSEGMENTATION_H

#include <QWidget>

#include <ui_marker_segmentation.h>

class BScanSegmentation;

class BScanSegLocalOpPaint;
class BScanSegLocalOpThreshold;
class BScanSegLocalOpOperation;


namespace BScanSegmentationMarker { class ThresholdData; }

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

	BScanSegmentation* segmentation;

	WGSegmentationThreshold thresSeries;
	WGSegmentationThreshold thresBScan ;
	WGSegmentationThreshold thresLocal ;


	BScanSegLocalOpPaint*     localOpPaint     = nullptr;
	BScanSegLocalOpThreshold* localOpThreshold = nullptr;
	BScanSegLocalOpOperation* localOpOperation = nullptr;


	void createConnections();

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
};

#endif // WGSEGMENTATION_H
