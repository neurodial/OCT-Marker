#include "numberpushbutton.h"

#include <QPainter>


void NumberPushButton::paintEvent(QPaintEvent* p)
{
	QPushButton::paintEvent(p);
	QPainter paint(this);

	int w = width();
	int h = height();

	QRect rect(w-h, 0, h, h);
	paint.drawText(rect, Qt::AlignCenter, additionalText);
}
