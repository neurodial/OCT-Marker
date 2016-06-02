#ifndef SLOIMAGE_H
#define SLOIMAGE_H

#include "cvimagewidget.h"

class SLOImageWidget : public CVImageWidget
{
public:
	SLOImageWidget();

    virtual ~SLOImageWidget();

protected:
	void paintEvent(QPaintEvent* event);
};

#endif // SLOIMAGE_H
