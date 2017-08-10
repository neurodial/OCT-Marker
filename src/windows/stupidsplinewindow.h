#pragma once

#include <QMainWindow>

class QAction;

class BScanMarkerWidget;
class ScrollAreaPan;
class QProgressDialog;

class BScanLayerSegmentation;

class StupidSplineWindow : public QMainWindow
{
	Q_OBJECT

	QDockWidget*          dwSloImage                  = nullptr;
	ScrollAreaPan*        bscanMarkerWidgetScrollArea = nullptr;
	BScanMarkerWidget*    bscanMarkerWidget           = nullptr;


// 	QProgressBar* loadProgressBar  = nullptr;
	QAction*      zoomInAction     = nullptr;
	QAction*      zoomOutAction    = nullptr;

	QProgressDialog* progressDialog = nullptr;


	bool loadFile(const QString& filename);
// 	bool addFile(const QString& filename);
// 	std::size_t loadFolder(const QString& foldername, int numMaxRecursiv = 10);

	virtual void closeEvent(QCloseEvent* e);

// 	void handleOpenUrl(const QUrl& url, bool singleInput);

	bool saveLayerSegmentation();
	bool copyLayerSegmentationFromOCTData();
	bool setIconsInMarkerWidget();

	BScanLayerSegmentation* getLayerSegmentationModul();
public:
	StupidSplineWindow(const char* filename = nullptr);
	~StupidSplineWindow();

	
// 	static void setMarkersStringList(QStringList& filters);

signals:
	void loadLastFile();

private slots:
	void zoomChanged(double zoom);
	void showAboutDialog();


	void loadFileStatusSlot(bool loading);
	void loadFileProgress(double frac);
};

