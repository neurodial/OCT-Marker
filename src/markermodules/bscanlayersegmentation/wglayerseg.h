#ifndef WGLAYERSEG_H
#define WGLAYERSEG_H


#include <QWidget>


class BScanLayerSegmentation;
class QButtonGroup;

class WGLayerSeg : public QWidget
{
	Q_OBJECT;

	BScanLayerSegmentation* parent;

	QButtonGroup* layerButtons = nullptr;

public:
	WGLayerSeg(BScanLayerSegmentation* parent);
	~WGLayerSeg();

private slots:

	void changeSeglineId(std::size_t index);
};

#endif // WGLAYERSEG_H
