#ifndef OCTMARKERMAINWINDOW_H
#define OCTMARKERMAINWINDOW_H

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


class OCTMarkerMainWindow : public QMainWindow
{
	Q_OBJECT

	void setupMenu();
	void setupStatusBar();

	void createMarkerToolbar();


	QDockWidget*       dwSloImage                  = nullptr;
	ScrollAreaPan*     bscanMarkerWidgetScrollArea = nullptr;
	BScanMarkerWidget* bscanMarkerWidget           = nullptr;


	QSpinBox*     bscanChooser     = nullptr;
	QLabel*       labelMaxBscan    = nullptr;
	QProgressBar* loadProgressBar  = nullptr;
	QAction*      zoomInAction     = nullptr;
	QAction*      zoomOutAction    = nullptr;

	QMenu* zoomMenu = nullptr;

	// void setActionToggel();
	void configBscanChooser();

	static void setMarkersFilters(QFileDialog& fd);

	bool loadFile(const QString& filename);
	bool addFile(const QString& filename);
	std::size_t loadFolder(const QString& foldername, int numMaxRecursiv = 10);

	virtual void closeEvent(QCloseEvent* e);

	void handleOpenUrl(const QUrl& url, bool singleInput);

protected:
	virtual void dropEvent     (QDropEvent     * event);
	virtual void dragEnterEvent(QDragEnterEvent* event);
	virtual void dragLeaveEvent(QDragLeaveEvent* event);
	virtual void dragMoveEvent (QDragMoveEvent * event);

public:
	OCTMarkerMainWindow(const char* filename = nullptr);
	~OCTMarkerMainWindow();

	
	static void setMarkersStringList(QStringList& filters);

signals:
	void loadLastFile();

private slots:
	void newCscanLoaded();
	void zoomChanged(double zoom);

	void loadFileStatusSlot(bool loading);
	void loadFileProgress(double frac);

public slots:

	virtual void showAboutDialog();
	virtual void showLoadImageDialog();
	virtual void showImportFromFolderDialog();

	// virtual void showAddMarkersDialog();
	virtual void showLoadMarkersDialog();
	virtual void showSaveMarkersDialog();

	virtual void saveMatlabBinCode();
	virtual void saveMatlabWriteBinCode();

	virtual void strechBScanImage2MaxWidth();
	virtual void strechBScanImage2MaxHeight();

};


class OptionInt;
class SendInt : public QObject
{
Q_OBJECT
	int v;
public:
	SendInt(int value) : v(value) {}
	void connectOptions(OptionInt& option, QAction* action);
public slots:
	void recive(bool b = true) { if(b) send(v); }
signals:
	void send(int value);
};


#endif // OCTMARKERMAINWINDOW_H
