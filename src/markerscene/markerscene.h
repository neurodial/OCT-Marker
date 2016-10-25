#ifndef MARKERSCENE_H
#define MARKERSCENE_H

#include <QGraphicsScene>
class RectItem;

class MarkerScene : public QGraphicsScene
{
public:
	GraphicsScene(QObject *parent = 0);

protected:
	// mouse events for creating/resizing items
	virtual void mousePressEvent(QGraphicsSceneMouseEvent *event);
	virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
	virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

private:
	RectItem	*m_currentRect;
};

#endif // MARKERSCENE_H
