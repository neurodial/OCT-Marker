/*
 * Copyright (c) 2018 Kay Gawlik <kaydev@amarunet.de> <kay.gawlik@beuth-hochschule.de> <kay.gawlik@charite.de>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

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
