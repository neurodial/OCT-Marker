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

#ifndef SLOIMAGEWIDGET_H
#define SLOIMAGEWIDGET_H

#include "cvimagewidget.h"
// #include <vector>
#include<data_structure/point2d.h>

namespace OctData
{
	class Series;
	class BScan;

	class CoordSLOpx;
	class ScaleFactor;
	class CoordTransform;
}

class QGraphicsScene;
class QGraphicsView;

// class QColor;
class QPainter;
class OctMarkerManager;
class SloMarkerBase;

class QWheelEvent;

class SloCoordTranslator;

class SLOImageWidget : public CVImageWidget
{
	Q_OBJECT

	struct Point {bool show = false; Point2DInt p; };

	Point markPos;

	OctMarkerManager& markerManger;
	QGraphicsView*  gv    = nullptr;
	QGraphicsScene* scene = nullptr;

	int clipX1 = 0;
	int clipY1 = 0;
	
	// std::vector<QColor*> intervallColors;
	bool drawBScans       = true;
	bool drawOnylActBScan = true;
	bool drawConvexHull   = true;
	bool singelBScanScan  = false;

// 	void createIntervallColors();
// 	void deleteIntervallColors();

	int getBScanNearPos(int x, int y, double tol);

	void updateGraphicsViewSize();
	void clipAndShowImage(const cv::Mat& img);

public:
	explicit SLOImageWidget(QWidget* parent = 0);

	virtual ~SLOImageWidget();

	bool getShowBScans() const                                   { return drawBScans; }

	virtual void setImageSize(QSize size) override;

protected:
	virtual void wheelEvent       (QWheelEvent*) override;

public slots:
	void showBScans(bool show);
	void showOnylActBScan(bool show);

	void showPosOnBScan(double t);

protected:
	void paintEvent(QPaintEvent* event) override;
	virtual void mousePressEvent(QMouseEvent*) override;

	void paintBScan      (QPainter& painter, const OctData::BScan& bscan, const SloCoordTranslator& transform, std::size_t bscanNr, bool paintMarker);
	void paintBScanLine  (QPainter& painter, const OctData::BScan& bscan, const SloCoordTranslator& transform, std::size_t bscanNr, bool paintMarker);
	void paintBScanCircle(QPainter& painter, const OctData::BScan& bscan, const SloCoordTranslator& transform, std::size_t bscanNr, bool paintMarker);

	void paintAnalyseGrid(QPainter& painter, const OctData::Series* series);
	void paintBScans     (QPainter& painter, const OctData::Series* series);
	void paintConvexHull (QPainter& painter, const OctData::Series* series);


	void saveLatexImage(const QString& filename) const;

	void resizeEvent(QResizeEvent* event) override;

private slots:
	void reladSLOImage();
	void bscanChanged(int);
	void sloMarkerChanged(SloMarkerBase* marker);
	void sloViewChanged  ();

	void setBScanVisibility(int opt);
	void updateMarkerOverlayImage();

	void saveLatexImageSlot();
};

#endif // SLOIMAGE_H

