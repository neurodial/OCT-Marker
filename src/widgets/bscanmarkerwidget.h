#ifndef BSCANMARKERWIDGET_H
#define BSCANMARKERWIDGET_H

#include "cvimagewidget.h"

class MarkerManager;
class BScanMarkerWidget : public CVImageWidget
{
	Q_OBJECT

	MarkerManager& markerManger;

public:
	BScanMarkerWidget(MarkerManager& markerManger);

    virtual ~BScanMarkerWidget();

protected:
	void paintEvent(QPaintEvent* event);

private slots:
	void bscanChanged(int);
	void cscanLoaded();
};

#endif // BSCANMARKERWIDGET_H
