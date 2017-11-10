#ifndef OBJECTSMARKERSCENE_H
#define OBJECTSMARKERSCENE_H

#include<QGraphicsScene>
#include<vector>

class RectItem;
class ObjectsmarkerFactory;

class ObjectsmarkerScene : public QGraphicsScene
{
	Q_OBJECT

	const ObjectsmarkerFactory& factory;

	bool addObjectMode = false;
	RectItem* newaddedItem = nullptr;

	void endInsertItem();
public:
	ObjectsmarkerScene(const ObjectsmarkerFactory& factory, QObject* parent = nullptr) : QGraphicsScene(parent), factory(factory) {};
	~ObjectsmarkerScene();

	void markersToList  (std::vector<RectItem*>& itemslist);
	void markersFromList(std::vector<RectItem*>& itemslist);

public slots:
	void setAddObjectMode(bool v);

signals:
	void addObjectModeChanged(bool v);

protected:
	virtual void keyPressEvent    (QKeyEvent               * event) override;
	virtual void mousePressEvent  (QGraphicsSceneMouseEvent* event) override;
	virtual void mouseMoveEvent   (QGraphicsSceneMouseEvent* event) override;
	virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;
};

#endif // OBJECTSMARKERSCENE_H
