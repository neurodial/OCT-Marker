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

#include<QWidget>
#include"distancemeter.h"

class QLabel;

class DistanceMeterPositionWidget : public QWidget
{
	Q_OBJECT

	typedef DistanceMeter::Position Position;

	QLabel* xPosLabel;
	QLabel* yPosLabel;
	QLabel* zPosLabel;
public:
	DistanceMeterPositionWidget(QWidget* parent);

public slots:
	void showPosition(const Position& p);
};

class DistanceMeterDistanceWidget : public QWidget
{
	Q_OBJECT

	typedef DistanceMeter::Distance Distance;

	QLabel* labelMillimeter     ;
	QLabel* labelBscanMillimeter;
	QLabel* labelBscanPx        ;
public:
	DistanceMeterDistanceWidget(QWidget* parent);

public slots:
	void showDistance(const Distance& d);
};

class WGDistanceMeter : public QWidget
{
	Q_OBJECT

	DistanceMeterPositionWidget* startPos;
	DistanceMeterPositionWidget* actualPos;
	DistanceMeterPositionWidget* clickPos;

	DistanceMeterDistanceWidget* actualDistance;
	DistanceMeterDistanceWidget* clickDistance;
public:
	WGDistanceMeter(DistanceMeter* parent);
	virtual ~WGDistanceMeter();
};
