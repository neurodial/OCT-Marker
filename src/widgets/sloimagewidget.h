#ifndef SLOIMAGEWIDGET_H
#define SLOIMAGEWIDGET_H

#include "cvimagewidget.h"

class MarkerManager;
class SLOImageWidget : public CVImageWidget
{
	Q_OBJECT

	MarkerManager& markerManger;

public:
	SLOImageWidget(MarkerManager& markerManger);

    virtual ~SLOImageWidget();

protected:
	void paintEvent(QPaintEvent* event);

private slots:
	void reladSLOImage();
	void bscanChanged(int);
};

#endif // SLOIMAGE_H

