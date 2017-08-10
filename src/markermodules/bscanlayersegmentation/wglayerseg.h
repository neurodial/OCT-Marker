#ifndef WGLAYERSEG_H
#define WGLAYERSEG_H


#include <QWidget>
#include<vector>


class BScanLayerSegmentation;
class QButtonGroup;
class QPushButton;
class QAction;
class QToolButton;

class WGLayerSeg : public QWidget
{
	Q_OBJECT;

	BScanLayerSegmentation* parent;

	QButtonGroup* layerButtons = nullptr;
	std::vector<QPushButton*> seglineButtons;

	QWidget* createMarkerToolButtons();

	QAction* actionMarkerMethodPen    = nullptr;
	QAction* actionMarkerMethodSpline = nullptr;

	QToolButton* buttonMarkerMethodPen    = nullptr;
	QToolButton* buttonMarkerMethodSpline = nullptr;

public:
	WGLayerSeg(BScanLayerSegmentation* parent);
	~WGLayerSeg();

	void setIconsToSimple(int size);

private slots:

	void changeSeglineId(std::size_t index);

	void markerMethodChanged();

	void setMarkerMethodPen();
	void setMarkerMethodSpline();
};

#endif // WGLAYERSEG_H
