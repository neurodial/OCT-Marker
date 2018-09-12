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

class ScaleFactor
{
	double x      = 1;
	double y      = 1;
	double factor = 1;
public:
	ScaleFactor(double x, double y) : x(x), y(y) {}
	ScaleFactor() = default;

	double getFactorX() const                                       { return x*factor; }
	double getFactorY() const                                       { return y*factor; }

	double getPureFactorX() const                                   { return x; }
	double getPureFactorY() const                                   { return y; }

	void setFactorX(double v)                                       { x = v; }
	void setFactorY(double v)                                       { y = v; }
	void setFactor(double v)                                        { factor = v; }


	bool isIdentical() const                                        { return x == 1.0 && y == 1.0 && factor == 1.0; }
	bool isValid()     const                                        { return x  > 0.0 && y  > 0.0 && factor  > 0.0; }
};
