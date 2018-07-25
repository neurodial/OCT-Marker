#ifndef OCTMARKERACTIONS_H
#define OCTMARKERACTIONS_H

#include<QObject>

class QAction;
class QMenu;
class QActionGroup;

class BScanMarkerWidget;
class CVImageWidget;


class OctMarkerActions : public QObject
{
	Q_OBJECT

	BScanMarkerWidget* bscanMarkerWidget;
	QAction* zoomInAction        = nullptr;
	QAction* zoomOutAction       = nullptr;
	QMenu  * zoomMenu            = nullptr;
	QAction* zoomFitHeightAction = nullptr;
	QAction* zoomFitWidthAction  = nullptr;

	QAction* undoAction          = nullptr;
	QAction* redoAction          = nullptr;

	QAction* aboutDialogAction   = nullptr;

	QActionGroup* bscanAspectRatioActions = nullptr;



	void addZoomAction(int zoom, CVImageWidget* bscanMarkerWidget, QMenu& menue);

public:
	OctMarkerActions(BScanMarkerWidget* bscanMarkerWidget);

	QAction* getZoomInAction       ();
	QAction* getZoomOutAction      ();
	QAction* getZoomMenuAction     ();
	QAction* getZoomFitHeightAction();
	QAction* getZoomFitWidthAction ();

	QAction* getUndoAction();
	QAction* getRedoAction();

	QAction* getAboutDialogAction  ();

	QActionGroup* getBscanAspectRatioActions();

private slots:
	void updateRedoUndo();
	void updateZoom(double zoom);

	void showAboutDialog();
};

#endif // OCTMARKERACTIONS_H
