#ifndef DWMARKERWIDGETS_H
#define DWMARKERWIDGETS_H

#include <QDockWidget>


// class QStackedWidget;

class OctMarkerManager;
class BscanMarkerBase;

class DWMarkerWidgets : public QDockWidget
{
	Q_OBJECT

public:
	explicit DWMarkerWidgets(QWidget* parent);
	virtual ~DWMarkerWidgets();

private slots:
	void markerChanged(BscanMarkerBase* marker);
};

#endif // DWMARKERWIDGETS_H
