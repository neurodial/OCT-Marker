#include "scrollareapan.h"


#include <QMouseEvent>
#include <QScrollBar>

void ScrollAreaPan::mousePressEvent(QMouseEvent* e)
{
	if(e->modifiers() & Qt::ControlModifier)
	{
		mousePos = e->pos();
		setPan(true);
		e->accept();
	}
	QScrollArea::mousePressEvent(e);
}


void ScrollAreaPan::mouseMoveEvent(QMouseEvent* e)
{
	if(paning)
	{
		if(e->modifiers() & Qt::ControlModifier)
		{
			QPoint diff = e->pos() - mousePos;
			mousePos    = e->pos();
			verticalScrollBar  ()->setValue(verticalScrollBar  ()->value() - diff.y());
			horizontalScrollBar()->setValue(horizontalScrollBar()->value() - diff.x());
			e->accept();
		}
		else
			setPan(false);
	}
	QScrollArea::mouseMoveEvent(e);
}

void ScrollAreaPan::mouseReleaseEvent(QMouseEvent* e)
{
	if(paning)
	{
		setPan(false);
		e->accept();
	}
}


void ScrollAreaPan::setPan(bool pan)
{
	paning = pan;
	if(pan)
	{
		setCursor(Qt::ClosedHandCursor);
	}
	else
		unsetCursor();
}
