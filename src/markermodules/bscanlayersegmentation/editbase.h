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

#ifndef EDITBASE_H
#define EDITBASE_H

#include<octdata/datastruct/segmentationlines.h>

#include"../bscanmarkerbase.h"

class QMouseEvent;
class QPainter;
class QRect;
class QContextMenuEvent;

class BScanLayerSegmentation;
class BScanMarkerWidget;
class ScaleFactor;

class EditBase
{
	BScanLayerSegmentation* parent;
public:
	EditBase(BScanLayerSegmentation* parent) : parent(parent) {}

	virtual void drawMarker(QPainter& /*painter*/, BScanMarkerWidget* /*widget*/, const QRect& /*drawrect*/, const ScaleFactor& /*scaleFactor*/) const {};

	virtual BscanMarkerBase::RedrawRequest mouseMoveEvent   (QMouseEvent*, BScanMarkerWidget*) { return BscanMarkerBase::RedrawRequest(); };
	virtual BscanMarkerBase::RedrawRequest mousePressEvent  (QMouseEvent*, BScanMarkerWidget*) { return BscanMarkerBase::RedrawRequest(); };
	virtual BscanMarkerBase::RedrawRequest mouseReleaseEvent(QMouseEvent*, BScanMarkerWidget*) { return BscanMarkerBase::RedrawRequest(); };
	virtual void contextMenuEvent(QContextMenuEvent* /*event*/) {}

	virtual bool keyPressEvent(QKeyEvent*, BScanMarkerWidget*)      { return false;}

	virtual void segLineChanged(OctData::Segmentationlines::Segmentline* /*segLine*/) {};

protected:
	int getBScanWidth() const;
	int getBScanHight() const;
	void requestFullUpdate();

	void rangeModified(std::size_t ascanBegin, std::size_t ascanEnd);
};

#endif // EDITBASE_H
