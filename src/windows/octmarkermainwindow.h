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

};

#endif // OCTMARKERMAINWINDOW_H
