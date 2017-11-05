#ifndef OBJECTSMARKERSCENE_H
#define OBJECTSMARKERSCENE_H

#include <QGraphicsScene>

class RectItem;

class ObjectsmarkerScene : public QGraphicsScene
{
	Q_OBJECT

	bool addObjectMode = false;
	RectItem* newaddedItem = nullptr;
public:
	ObjectsmarkerScene(QObject* parent = nullptr) : QGraphicsScene(parent) {};
	~ObjectsmarkerScene();


public slots:
	void setAddObjectMode(bool v) { addObjectMode = v; }

protected:
	virtual void keyPressEvent    (QKeyEvent               * event) override;
	virtual void mousePressEvent  (QGraphicsSceneMouseEvent* event) override;
	virtual void mouseMoveEvent   (QGraphicsSceneMouseEvent* event) override;
	virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;
};

#endif // OBJECTSMARKERSCENE_H
