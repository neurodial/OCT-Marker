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

#ifndef WGLAYERSEG_H
#define WGLAYERSEG_H


#include <QWidget>
#include<vector>


class BScanLayerSegmentation;
class QButtonGroup;
class QPushButton;
class QAction;
class QToolButton;
class QComboBox;

class WGLayerSeg : public QWidget
{
	Q_OBJECT;

	BScanLayerSegmentation* parent;

	QButtonGroup* layerButtons = nullptr;
	std::vector<QPushButton*> seglineButtons;


	QAction* actionMarkerMethodPen    = nullptr;
	QAction* actionMarkerMethodSpline = nullptr;
	QAction* actionShowSeglines       = nullptr;
	QAction* actionUpdateThicknessmap = nullptr;

	QToolButton* buttonMarkerMethodPen    = nullptr;
	QToolButton* buttonMarkerMethodSpline = nullptr;
	QToolButton* buttonShowSeglines       = nullptr;

	QComboBox* thicknessmapTemplates = nullptr;

	void createMarkerToolButtons(QLayout& layout);
	void addLayerButtons(QLayout& layout);
	void addThicknessMapControls(QLayout& layout);

public:
	WGLayerSeg(BScanLayerSegmentation* parent);
	~WGLayerSeg();

	void setIconsToSimple(int size);

private slots:
	void markerMethodChanged();

	void setMarkerMethodPen();
	void setMarkerMethodSpline();

	void thicknessmapTemplateChanged(int index);

	void segLineIdChanged(std::size_t index);
	void segLineVisibleChanged(bool v);
};

#endif // WGLAYERSEG_H
