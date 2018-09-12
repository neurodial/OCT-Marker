/*
 * Copyright (c) 2018 Kay Gawlik <kaydev@amarunet.de> <kay.gawlik@beuth-hochschule.de> <kay.gawlik@charite.de>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

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
