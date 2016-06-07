#ifndef SLOIMAGEWIDGET_H
#define SLOIMAGEWIDGET_H

#include "cvimagewidget.h"
#include <vector>

class QColor;
class MarkerManager;

class SLOImageWidget : public CVImageWidget
{
	Q_OBJECT

	MarkerManager& markerManger;
	
	std::vector<QColor*> intervallColors;

	void createIntervallColors();
	void deleteIntervallColors();
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

