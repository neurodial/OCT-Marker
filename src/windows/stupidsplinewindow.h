#pragma once

#include <QMainWindow>

class QAction;
class QSpinBox;
class QLabel;
class QToolButton;

class BScanMarkerWidget;
class ScrollAreaPan;
class QProgressDialog;

class BScanLayerSegmentation;
class PaintMarker;

class StupidSplineWindow : public QMainWindow
{
	Q_OBJECT

	QDockWidget*          dwSloImage                  = nullptr;
	ScrollAreaPan*        bscanMarkerWidgetScrollArea = nullptr;
	BScanMarkerWidget*    bscanMarkerWidget           = nullptr;

	QSize buttonSize = QSize(40, 40);

// 	QProgressBar* loadProgressBar  = nullptr;
	QAction*      zoomInAction     = nullptr;
	QAction*      zoomOutAction    = nullptr;
	QAction*      zoomFitAction    = nullptr;

	QProgressDialog* progressDialog = nullptr;

	QSpinBox*     bscanChooser     = nullptr;
	QLabel*       labelMaxBscan    = nullptr;


	QToolButton* buttonUndo = nullptr;
	QToolButton* buttonRedo = nullptr;

	PaintMarker* pmm = nullptr;

	bool saved = false;

// 	bool addFile(const QString& filename);
// 	std::size_t loadFolder(const QString& foldername, int numMaxRecursiv = 10);

	virtual void closeEvent(QCloseEvent* e);

// 	void handleOpenUrl(const QUrl& url, bool singleInput);

	bool saveLayerSegmentation();
	bool copyLayerSegmentationFromOCTData();
	bool setIconsInMarkerWidget();

	bool hasDataChange();

	QDockWidget* createStupidControls();

	void updateWindowTitle();

	BScanLayerSegmentation* getLayerSegmentationModul();
public:
	StupidSplineWindow();
	~StupidSplineWindow();

	bool loadFile(const QString& filename);
	
// 	static void setMarkersStringList(QStringList& filters);

signals:
	void loadLastFile();

private slots:
	void zoomChanged(double zoom);
	void showAboutDialog();

	void saveAndClose();

	void loadFileStatusSlot(bool loading);
	void loadFileProgress(double frac);

	void fitBScanImage2Widget();
	void updateBScanChooser();
	void updateRedoUndoButtons();
};

