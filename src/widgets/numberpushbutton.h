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

#ifndef NUMBERPUSHBUTTON_H
#define NUMBERPUSHBUTTON_H

#include <QPushButton>

class NumberPushButton : public QPushButton
{
	Q_OBJECT

	QString additionalText;
public:
	NumberPushButton(int number, QWidget *parent = nullptr)
	: QPushButton(parent)
	, additionalText(QString::number(number))
	{}

	NumberPushButton(const QString& number, QWidget *parent = nullptr)
	: QPushButton(parent)
	, additionalText(number)
	{}

	void paintEvent(QPaintEvent* p) override;
};


#endif // NUMBERPUSHBUTTON_H
