#include "editspline.h"



#include<QPainter>


namespace
{
	void paintPolygon(QPainter& painter, const std::vector<Point2D>& polygon, double factor)
	{
		if(polygon.size() < 2)
			return;

		      std::vector<Point2D>::const_iterator it    = polygon.begin();
		const std::vector<Point2D>::const_iterator itEnd = polygon.end();
		Point2D lastPoint = *it;
		++it;
		while(it != itEnd)
		{
			painter.drawEllipse(lastPoint.getX()*factor-4, lastPoint.getY()*factor-4, 8, 8);
// 			painter.drawLine(lastPoint.getX()*factor, lastPoint.getY()*factor, it->getX()*factor, it->getY()*factor);
			lastPoint = *it;
			++it;
		}
		painter.drawEllipse(lastPoint.getX()*factor, lastPoint.getY()*factor, 5, 5);
	}

}

void EditSpline::drawMarker(QPainter& painter, BScanMarkerWidget* widget, const QRect&) const
{
}

bool EditSpline::mouseMoveEvent(QMouseEvent*, BScanMarkerWidget*)
{
}

bool EditSpline::mousePressEvent(QMouseEvent*, BScanMarkerWidget*)
{
}

bool EditSpline::mouseReleaseEvent(QMouseEvent*, BScanMarkerWidget*)
{
}


