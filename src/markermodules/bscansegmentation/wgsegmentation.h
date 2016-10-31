#ifndef WGSEGMENTATION_H
#define WGSEGMENTATION_H

#include <QWidget>

#include <ui_marker_segmentation.h>

class BScanSegmentation;

class WGSegmentation : public QWidget, Ui::BScanSegmentation
{
	Q_OBJECT;

	::BScanSegmentation* segmentation;

	void createConnections();

public:
	WGSegmentation(::BScanSegmentation* parent);
	virtual ~WGSegmentation();


private slots:
	void slotSeriesInitFromSeg();
	void slotSeriesInitFromThresh();

	void slotBscanInitFromThresh();
	void slotBscanErode();
	void slotBscanDilate();
	void slotBscanOpenClose();
	void slotBscanMedian();

	void slotLocalThresh();
	void slotLocalPaint();
	void slotLocalOperation();
};

#endif // WGSEGMENTATION_H
