#pragma once

#include <QMainWindow>

class QLabel;
class QSpinBox;
class QAction;
class QFileDialog;
class QProgressBar;
class QUrl;
class QMenu;

class BScanMarkerWidget;
class DWSloImage;
class CScan;
class ScrollAreaPan;
class DWDebugOutput;


class StupidSplineWindow : public QMainWindow
{
	Q_OBJECT

	static void messageOutput(QtMsgType type, const QMessageLogContext& context, const QString& msg);

	void setupStatusBar();


	QDockWidget*          dwSloImage                  = nullptr;
	ScrollAreaPan*        bscanMarkerWidgetScrollArea = nullptr;
	BScanMarkerWidget*    bscanMarkerWidget           = nullptr;
	static DWDebugOutput* dwDebugOutput;


	QSpinBox*     bscanChooser     = nullptr;
	QLabel*       labelMaxBscan    = nullptr;
	QProgressBar* loadProgressBar  = nullptr;
	QAction*      zoomInAction     = nullptr;
	QAction*      zoomOutAction    = nullptr;

	QMenu* zoomMenu = nullptr;


	static void setMarkersFilters(QFileDialog& fd);


	bool loadFile(const QString& filename);
// 	bool addFile(const QString& filename);
// 	std::size_t loadFolder(const QString& foldername, int numMaxRecursiv = 10);

	virtual void closeEvent(QCloseEvent* e);

// 	void handleOpenUrl(const QUrl& url, bool singleInput);

public:
	StupidSplineWindow(const char* filename = nullptr);
	~StupidSplineWindow();

	
// 	static void setMarkersStringList(QStringList& filters);

signals:
	void loadLastFile();

private slots:
	void newCscanLoaded();
	void zoomChanged(double zoom);

	void loadFileStatusSlot(bool loading);
	void loadFileProgress(double frac);

};

