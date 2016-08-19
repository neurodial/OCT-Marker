/*
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

#ifndef RECTITEM_H
#define RECTITEM_H

#include<QGraphicsRectItem>

class RectItem : public QGraphicsRectItem
{

public:
	RectItem();

	void makeValid();
	void setDescription(const QString& text)                         { descripton = text; }
	virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;

protected:
	virtual void mousePressEvent  (QGraphicsSceneMouseEvent* event) override;
	virtual void mouseMoveEvent   (QGraphicsSceneMouseEvent* event) override;
	virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;

private:
	QString descripton;
	bool resizing = false;
	QPointF mousePressPos;
	int activeRezieHandle = 0;
	int minSize = 5;

	constexpr static const double sizeResizeHandler = 0.2;

	int insideHandler(double relPos) const;

};

#endif // RECTITEM_H
