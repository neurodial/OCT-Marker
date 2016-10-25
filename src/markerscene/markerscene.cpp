#include "markerscene.h"

MarkerScene::GraphicsScene(QObject* parent)
{

}


void MarkerScene::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{

}


void MarkerScene::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
	if(event->button() != Qt::LeftButton)
	{
		QGraphicsScene::mousePressEvent(event);
		return;
	}

	// check if an existing item has been clicked
	for(QGraphicsItem* itm : items())
	{
		if(itm->sceneBoundingRect().contains(event->scenePos()))
		{
			QGraphicsScene::mousePressEvent(event);
			update();
			return;
		}
	}

	// clear focus/selection of any existing items
	if(focusItem())
		focusItem()->clearFocus();
	
	for(QGraphicsItem *itm : selectedItems())
		itm->setSelected(false);

	// create new item with size 0
	m_currentRect = new RectItem();
	m_currentRect->setRect(QRectF(event->scenePos(), QSizeF(0, 0)));
	m_currentRect->setSelected(true);
	addItem(m_currentRect);

	update();

}


void MarkerScene::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{

}

