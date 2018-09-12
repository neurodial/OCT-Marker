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

#pragma once

#include<QSlider>


class DoubleSlider : public QSlider
{
	Q_OBJECT

	const double factor;
public:
	DoubleSlider(int factor, QWidget* parent)
	: QSlider(parent)
	, factor(factor)
	{
		connect(this, &QSlider::valueChanged, this, &DoubleSlider::vChanged);
	}

	DoubleSlider(Qt::Orientation orientation, int factor, QWidget* parent)
	: QSlider(orientation, parent)
	, factor(factor)
	{
		connect(this, &QSlider::valueChanged, this, &DoubleSlider::vChanged);
	}


	void setRange(double min, double max)
	{
		QSlider::setRange(static_cast<int>(min*factor)
		                , static_cast<int>(max*factor));
	}

private slots:
	void vChanged(int v) { emit(valueChanged(v/factor)); }

public slots:
	void setValue(double v) { QSlider::setValue(static_cast<int>(v*factor)); }

signals:
	void valueChanged(double);

};
