#ifndef BSCANMARKERWIDGET_H
#define BSCANMARKERWIDGET_H

#include "cvimagewidget.h"

#include <vector>

class QWheelEvent;
class QMouseEvent;
class MarkerManager;
class QColor;

class BScanMarkerWidget : public CVImageWidget
{
	Q_OBJECT

	int clickPos;

	MarkerManager& markerManger;

	std::vector<QColor*> intervallColors;

	void createIntervallColors();
	void deleteIntervallColors();

public:
	BScanMarkerWidget(MarkerManager& markerManger);

	virtual ~BScanMarkerWidget();

    virtual void wheelEvent(QWheelEvent*);

protected:
	void paintEvent(QPaintEvent* event);

    virtual void mouseMoveEvent   (QMouseEvent*);
    virtual void mousePressEvent  (QMouseEvent*);
    virtual void mouseReleaseEvent(QMouseEvent*);

private slots:
	void bscanChanged(int);
	void cscanLoaded();

signals:
	void bscanChangeInkrement(int delta);
};

#endif // BSCANMARKERWIDGET_H
