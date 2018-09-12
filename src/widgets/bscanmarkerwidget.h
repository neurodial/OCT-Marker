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

#ifndef BSCANMARKERWIDGET_H
#define BSCANMARKERWIDGET_H

#include "cvimagewidget.h"

#include <QPoint>

namespace OctData { class BScan; }

class QWheelEvent;
class QMouseEvent;
class QGraphicsScene;
class QGraphicsView;

class OctMarkerManager;
class BscanMarkerBase;

class ContureSegment;

class PaintMarker;

class BScanMarkerWidget : public CVImageWidget
{
	Q_OBJECT

	OctMarkerManager& markerManger;
	QGraphicsView*  gv    = nullptr;
	QGraphicsScene* scene = nullptr;

	
	QAction*                                saveRawImageAction = nullptr;
	QAction*                                saveRawMatAction   = nullptr;
	QAction*                                saveRawBinAction   = nullptr;
	QAction*                                saveImageBinAction = nullptr;
// 	const OctData::BScan*                   actBscan           = nullptr;
	const PaintMarker*                      paintMarker        = nullptr;

	bool controlUsed = false;
	double bscanAspectRatio = 1.;
	void fitAspectRatio();
	
	int fdSaveRaw(QString& filename);
	void updateRawExport();
	bool existsRaw(const OctData::BScan* bscan) const;
	bool rawSaveableAsImage() const;

	bool checkControlUsed(QMouseEvent* event);
	bool checkControlUsed(QKeyEvent  * event);
	bool checkControlUsed(bool modPressed);

	void paintConture(QPainter& painter, const std::vector<ContureSegment>& contours) const;
	void paintSegmentations(QPainter& segPainter, const ScaleFactor& scaleFactor) const;


	void transformCoordWidget2Img(int xWidget, int yWidget, int& xImg, int& yImg)
	{
		const ScaleFactor& factor = getImageScaleFactor();
		yImg = static_cast<int>(yWidget/factor.getFactorX() + 0.5);
		xImg = static_cast<int>(xWidget/factor.getFactorY() + 0.5);
	}

public:
	BScanMarkerWidget();

	virtual ~BScanMarkerWidget();

	static void paintSegmentationLine(QPainter& segPainter, int bScanHeight, const std::vector<double>& segLine, const ScaleFactor& factor);

	void setPaintMarker(const PaintMarker* pm);

protected:
	virtual void paintEvent(QPaintEvent* event) override;
	virtual void contextMenuEvent(QContextMenuEvent* event) override;


    bool event(QEvent *event) override;

	virtual void mouseMoveEvent   (QMouseEvent*) override;
	virtual void mousePressEvent  (QMouseEvent*) override;
	virtual void mouseReleaseEvent(QMouseEvent*) override;

	virtual void keyPressEvent    (QKeyEvent*  ) override;
	virtual void keyReleaseEvent  (QKeyEvent*  ) override;
	virtual void leaveEvent       (QEvent*     ) override;
	virtual void wheelEvent       (QWheelEvent*) override;

	virtual void resizeEvent      (QResizeEvent* e) override { CVImageWidget::resizeEvent(e); triggerAutoImageFit(); }


private slots:
	void imageChanged();
	void cscanLoaded();

	void viewOptionsChangedSlot();
	void updateGraphicsViewSize();

	void markersMethodChanged(BscanMarkerBase* marker);
	
	void triggerAutoImageFit();

	void updateAspectRatio();

public slots:
	virtual void saveRawImage();
	virtual void saveRawMat  ();
	virtual void saveRawBin  ();
	virtual void saveImageBin();

	virtual void saveLatexImage();

	virtual void showImage(const cv::Mat& image) override;
	virtual void setZoom(double factor) override;
	
signals:
	void bscanChangeInkrement(int delta);
	void mousePosInImage(int x, int y);
	void mouseLeaveImage();
	void mousePosOnBScan(double t);

};

#endif // BSCANMARKERWIDGET_H
