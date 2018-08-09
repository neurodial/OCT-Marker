#pragma once

#include <QMainWindow>
#include <QWidgetAction>

class QAction;
class QSpinBox;
class QLabel;
class QToolButton;

class BScanMarkerWidget;
class ScrollAreaPan;
class QProgressDialog;

class BScanLayerSegmentation;
class PaintMarker;
class OptionDouble;

#include"octmarkeractions.h"




class StupidSplineWindow : public QMainWindow
{
	Q_OBJECT

	QDockWidget*          dwSloImage                  = nullptr;
	ScrollAreaPan*        bscanMarkerWidgetScrollArea = nullptr;
	BScanMarkerWidget*    bscanMarkerWidget           = nullptr;

	QSize buttonSize = QSize(40, 40);

	OctMarkerActions markerActions;


	QProgressDialog* progressDialog = nullptr;
	PaintMarker* pmm = nullptr;

	bool saved = false;

	virtual void closeEvent(QCloseEvent* e);


	bool saveLayerSegmentation();
	bool copyLayerSegmentationFromOCTData();
	bool setIconsInMarkerWidget();

	bool hasDataChange();

	QDockWidget* createStupidControls();

	void updateWindowTitle();

	QToolButton* genToolButton(QAction* action);
	QAction* getDoubleSliderAction(OptionDouble& option, const QString& label);

	BScanLayerSegmentation* getLayerSegmentationModul();
public:
	StupidSplineWindow();
	~StupidSplineWindow();

	bool loadFile(const QString& filename);
signals:
	void loadLastFile();

private slots:
	void saveAndClose();

	void loadFileStatusSlot(bool loading);
	void loadFileProgress(double frac);

	void setProgramOptions();
};

