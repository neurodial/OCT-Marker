#ifndef WGLAYERSEG_H
#define WGLAYERSEG_H


#include <QWidget>
#include<vector>


class BScanLayerSegmentation;
class QButtonGroup;
class QPushButton;
class QAction;
class QToolButton;
class QComboBox;

class WGLayerSeg : public QWidget
{
	Q_OBJECT;

	BScanLayerSegmentation* parent;

	QButtonGroup* layerButtons = nullptr;
	std::vector<QPushButton*> seglineButtons;


	QAction* actionMarkerMethodPen    = nullptr;
	QAction* actionMarkerMethodSpline = nullptr;
	QAction* actionShowSeglines       = nullptr;

	QToolButton* buttonMarkerMethodPen    = nullptr;
	QToolButton* buttonMarkerMethodSpline = nullptr;
	QToolButton* buttonShowSeglines       = nullptr;

	QComboBox* thicknessmapTemplates = nullptr;

	void createMarkerToolButtons(QLayout& layout);
	void addLayerButtons(QLayout& layout);
	void addThicknessMapControls(QLayout& layout);

public:
	WGLayerSeg(BScanLayerSegmentation* parent);
	~WGLayerSeg();

	void setIconsToSimple(int size);

private slots:

	void changeSeglineId(std::size_t index);

	void markerMethodChanged();

	void setMarkerMethodPen();
	void setMarkerMethodSpline();

	void thicknessmapTemplateChanged(int index);

	void segLineIdChanged(std::size_t index);
	void segLineVisibleChanged(bool v);
};

#endif // WGLAYERSEG_H
