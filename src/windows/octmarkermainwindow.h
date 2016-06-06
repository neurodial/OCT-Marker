#ifndef OCTMARKERMAINWINDOW_H
#define OCTMARKERMAINWINDOW_H

#include <QMainWindow>

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


public:
	OCTMarkerMainWindow();
	~OCTMarkerMainWindow();


public slots:

	virtual void showAboutDialog();
    virtual void showLoadImageDialog();

};

#endif // OCTMARKERMAINWINDOW_H
