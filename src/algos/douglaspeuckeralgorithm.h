/*
 * Copyright (c) 2018 Kay Gawlik <kaydev@amarunet.de>
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

#ifndef DOUGLASPEUCKERALGORITHM_H
#define DOUGLASPEUCKERALGORITHM_H

#include<vector>
#include<list>

#include<data_structure/point2d.h>

class DouglasPeuckerAlgorithm
{
	typedef std::list<Point2D>::iterator PtIt;
	typedef std::vector<Point2D>::const_iterator PtItSource;

	const double tol = 0.5;

	void divideOnPoint(const PtItSource firstPoint, const PtItSource dividePoint, const PtItSource lastPoint, PtIt insertPointBefore);
	void douglasPeuckerAlgorithmRecursiv(PtIt insertPointBefore, const PtItSource firstPoint, const PtItSource lastPoint);

	std::list<Point2D> destPoints;

public:
	DouglasPeuckerAlgorithm(const std::vector<Point2D>& values, double tol);


	const std::list<Point2D>& getPoints() const                     { return destPoints; }
};

#endif // DOUGLASPEUCKERALGORITHM_H
