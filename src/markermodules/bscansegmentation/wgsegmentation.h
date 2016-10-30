#ifndef WGSEGMENTATION_H
#define WGSEGMENTATION_H

#include <QWidget>

#include <ui_marker_segmentation.h>

class BScanSegmentation;

class WGSegmentation : public QWidget, Ui::BScanSegmentation
{
	Q_OBJECT;
public:
	WGSegmentation(::BScanSegmentation* parent);
	virtual ~WGSegmentation();
};

#endif // WGSEGMENTATION_H
