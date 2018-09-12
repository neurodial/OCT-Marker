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

#ifndef SCROLLAREAPAN_H
#define SCROLLAREAPAN_H

#include <QScrollArea>

class QInputEvent;

class ScrollAreaPan : public QScrollArea
{
	Q_OBJECT

	enum class PanStatus { None, PanReady, Paning };
	enum class MausButton { Undef, Pressed, Unpressed };

public:
	explicit ScrollAreaPan(QWidget *parent = Q_NULLPTR);

	int getVScrollbarWidth () const;
	int getHScrollbarHeight() const;
protected:
	void mousePressEvent  (QMouseEvent* e) override;
	void mouseMoveEvent   (QMouseEvent* e) override;
	void mouseReleaseEvent(QMouseEvent* e) override;
	void keyPressEvent    (QKeyEvent*   e) override;
	void keyReleaseEvent  (QKeyEvent*   e) override;
private:
	QPoint mousePos;
	PanStatus panStatus = PanStatus::None;

	bool updatePanStatus(bool modifierPressed, MausButton mouseButtonPressed);

public slots:
	void scrollTo(int x, int y);
};

#endif // SCROLLAREAPAN_H
