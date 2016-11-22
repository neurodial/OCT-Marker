#include "scrollareapan.h"


#include <QMouseEvent>
#include <QScrollBar>

#include <QDebug>

namespace
{
	inline bool modPressed(QKeyEvent* e)
	{
		return e->modifiers() == Qt::ControlModifier;
	}
	inline bool modPressed(QMouseEvent* e)
	{
		return e->modifiers() == Qt::ControlModifier;
	}
}

void ScrollAreaPan::mousePressEvent(QMouseEvent* e)
{
	QScrollArea::mousePressEvent(e);

	if((e->buttons() & Qt::LeftButton) == 0)
		return;

	if(upadtePanStatus(modPressed(e), MausButton::Pressed))
	{
		mousePos = e->pos();
		e->accept();
	}
}


void ScrollAreaPan::mouseMoveEvent(QMouseEvent* e)
{
	QScrollArea::mouseMoveEvent(e);
	if(panStatus == PanStatus::Paning)
	{
		if(modPressed(e))
		{
			QPoint diff = e->pos() - mousePos;
			mousePos    = e->pos();
			verticalScrollBar  ()->setValue(verticalScrollBar  ()->value() - diff.y());
			horizontalScrollBar()->setValue(horizontalScrollBar()->value() - diff.x());
			e->accept();
		}
		else
			upadtePanStatus(false, MausButton::Undef);
	}
}

void ScrollAreaPan::mouseReleaseEvent(QMouseEvent* e)
{
	if((e->buttons() & Qt::LeftButton) != 0)
		return;

	QScrollArea::mouseReleaseEvent(e);
	if(upadtePanStatus(modPressed(e), MausButton::Unpressed))
		e->accept();
}



void ScrollAreaPan::keyPressEvent(QKeyEvent* e)
{
	QScrollArea::keyPressEvent(e);
	upadtePanStatus(modPressed(e), MausButton::Undef);
}

void ScrollAreaPan::keyReleaseEvent(QKeyEvent* e)
{
	QScrollArea::keyReleaseEvent(e);
	upadtePanStatus(modPressed(e), MausButton::Undef);
}


bool ScrollAreaPan::upadtePanStatus(bool modifierPressed, MausButton mouseButtonPressed)
{
	if(!modifierPressed)
	{
		if(panStatus != PanStatus::None)
		{
			panStatus = PanStatus::None;
			unsetCursor();
			return true;
		}
		return false;
	}

	switch(panStatus)
	{
		case PanStatus::None:
			if(mouseButtonPressed == MausButton::Undef)
			{
				panStatus = PanStatus::PanReady;
				setCursor(Qt::OpenHandCursor);
				return true;
			}
			break;

		case PanStatus::PanReady:
			if(mouseButtonPressed == MausButton::Pressed)
			{
				panStatus = PanStatus::Paning;
				setCursor(Qt::ClosedHandCursor);
				return true;
			}
			break;

		case PanStatus::Paning:
			if(mouseButtonPressed == MausButton::Unpressed)
			{
				panStatus = PanStatus::PanReady;
				setCursor(Qt::OpenHandCursor);
				return true;
			}
			break;
	}

	return false;
}

