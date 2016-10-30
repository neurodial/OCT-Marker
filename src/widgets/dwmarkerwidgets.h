#ifndef DWMARKERWIDGETS_H
#define DWMARKERWIDGETS_H

#include <QDockWidget>


// class QStackedWidget;

class BScanMarkerManager;
class BscanMarkerBase;

class DWMarkerWidgets : public QDockWidget
{
	Q_OBJECT

    // QStackedWidget* stackedMarkerWidgets;
	BScanMarkerManager* markerManager;

public:
	explicit DWMarkerWidgets(BScanMarkerManager* markerManager, QWidget* parent);
	virtual ~DWMarkerWidgets();

private slots:
	void markerChanged(BscanMarkerBase* marker);
};

#endif // DWMARKERWIDGETS_H
