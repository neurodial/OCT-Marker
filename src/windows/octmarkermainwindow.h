#ifndef OCTMARKERMAINWINDOW_H
#define OCTMARKERMAINWINDOW_H

#include <QMainWindow>

class QLabel;
class QSpinBox;
class QAction;
class QFileDialog;
class BScanMarkerWidget;
class MarkerManager;
class DWSloImage;
class CScan;


class OCTMarkerMainWindow : public QMainWindow
{
	Q_OBJECT

	void setupMenu();

	void createMarkerToolbar();
	
	MarkerManager* markerManager = nullptr;

	DWSloImage*    dwSloImage  = nullptr;
	BScanMarkerWidget* bscanMarkerWidget = nullptr;

	QAction* fillMarkerAction  = nullptr;
	QAction* paintMarkerAction = nullptr;

	QSpinBox* bscanChooser     = nullptr;
	QLabel*   labelMaxBscan    = nullptr;

	std::vector<QAction*> markersActions;

	void setActionToggel();
	void configBscanChooser();

	static void setMarkersFilters(QFileDialog& fd);

	bool loadFile(const QString& filename);

	virtual void closeEvent(QCloseEvent* e);

protected:
	virtual void dropEvent     (QDropEvent     * event);
	virtual void dragEnterEvent(QDragEnterEvent* event);
	virtual void dragLeaveEvent(QDragLeaveEvent* event);
	virtual void dragMoveEvent (QDragMoveEvent * event);

public:
	OCTMarkerMainWindow();
	~OCTMarkerMainWindow();

	
	static void setMarkersStringList(QStringList& filters);

private slots:
	virtual void newCscanLoaded();

public slots:

	virtual void showAboutDialog();
    virtual void showLoadImageDialog();

	virtual void showAddMarkersDialog();
	virtual void showLoadMarkersDialog();
	virtual void showSaveMarkersDialog();

	virtual void saveMatlabBinCode();

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
