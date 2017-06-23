#ifndef EDITPEN_H
#define EDITPEN_H

#include"editbase.h"

#include<vector>


class EditPen : public EditBase
{
public:
	EditPen(BScanLayerSegmentation* base) : EditBase(base) {}
	virtual ~EditPen() {}


	virtual void drawMarker(QPainter& painter, BScanMarkerWidget* widget, const QRect& /*drawrect*/) const override;

	virtual bool mouseMoveEvent   (QMouseEvent*, BScanMarkerWidget*) override;
	virtual bool mousePressEvent  (QMouseEvent*, BScanMarkerWidget*) override;
	virtual bool mouseReleaseEvent(QMouseEvent*, BScanMarkerWidget*) override;


	virtual void segLineChanged(OctData::Segmentationlines::Segmentline& segLine) override;


};

#endif // EDITPEN_H

