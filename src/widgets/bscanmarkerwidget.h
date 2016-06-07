#ifndef BSCANMARKERWIDGET_H
#define BSCANMARKERWIDGET_H

#include "cvimagewidget.h"

#include <vector>
#include <QPoint>

class QWheelEvent;
class QMouseEvent;
class MarkerManager;
class QColor;

class BScanMarkerWidget : public CVImageWidget
{
	Q_OBJECT

	QPoint clickPos;
	QPoint mousePos;

	MarkerManager& markerManger;

	std::vector<QColor*> intervallColors;
	std::vector<QColor*> markerColors;

	void createIntervallColors();
	void deleteIntervallColors();
	
	bool markerActiv = false;

public:
	BScanMarkerWidget(MarkerManager& markerManger);

	virtual ~BScanMarkerWidget();

    virtual void wheelEvent(QWheelEvent*);

protected:
	void paintEvent(QPaintEvent* event);

	virtual void mouseMoveEvent   (QMouseEvent*);
	virtual void mousePressEvent  (QMouseEvent*);
	virtual void mouseReleaseEvent(QMouseEvent*);
	
	virtual void keyPressEvent    (QKeyEvent*);

private slots:
	void bscanChanged(int);
	void cscanLoaded();

signals:
	void bscanChangeInkrement(int delta);
};

#endif // BSCANMARKERWIDGET_H
