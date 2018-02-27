#ifndef OCTMARKERACTIONS_H
#define OCTMARKERACTIONS_H

#include<QObject>

class QAction;
class BScanMarkerWidget;


class OctMarkerActions : public QObject
{
	Q_OBJECT

	BScanMarkerWidget* bscanMarkerWidget;
	QAction* zoomInAction  = nullptr;
	QAction* zoomOutAction = nullptr;
	QAction* undoAction    = nullptr;
	QAction* redoAction    = nullptr;

public:
	OctMarkerActions(BScanMarkerWidget* bscanMarkerWidget);

	QAction* getZoomInAction ();
	QAction* getZoomOutAction();

	QAction* getUndoAction();
	QAction* getRedoAction();

private slots:
	void updateRedoUndo();

};

#endif // OCTMARKERACTIONS_H
