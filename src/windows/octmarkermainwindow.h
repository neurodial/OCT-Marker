#ifndef OCTMARKERMAINWINDOW_H
#define OCTMARKERMAINWINDOW_H

#include <QMainWindow>

class QAction;
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

	std::vector<QAction*> markersActions;

	void setActionToggel();

public:
	OCTMarkerMainWindow();
	~OCTMarkerMainWindow();

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
