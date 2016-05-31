#ifndef OCTMARKERMAINWINDOW_H
#define OCTMARKERMAINWINDOW_H

#include <qt4/QtGui/QMainWindow>

class CVImageWidget;

class OCTMarkerMainWindow : public QMainWindow
{
	Q_OBJECT

	void setupMenu();

	CVImageWidget* imageWidget;

public:
	OCTMarkerMainWindow();
	~OCTMarkerMainWindow();


public slots:

	virtual void showAboutDialog();
};

#endif // OCTMARKERMAINWINDOW_H
