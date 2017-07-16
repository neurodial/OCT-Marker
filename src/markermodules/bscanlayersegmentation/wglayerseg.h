#ifndef WGLAYERSEG_H
#define WGLAYERSEG_H


#include <QWidget>
#include<vector>


class BScanLayerSegmentation;
class QButtonGroup;
class QPushButton;

class WGLayerSeg : public QWidget
{
	Q_OBJECT;

	BScanLayerSegmentation* parent;

	QButtonGroup* layerButtons = nullptr;
	std::vector<QPushButton*> seglineButtons;

public:
	WGLayerSeg(BScanLayerSegmentation* parent);
	~WGLayerSeg();

private slots:

	void changeSeglineId(std::size_t index);
};

#endif // WGLAYERSEG_H
