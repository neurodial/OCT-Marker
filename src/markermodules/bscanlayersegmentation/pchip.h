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
#ifndef PCHIP_H
#define PCHIP_H

#include<vector>

#include<data_structure/point2d.h>

class PChip
{
	std::vector<double> values;
public:
	PChip(const std::vector<Point2D>& points, std::size_t length);


	const std::vector<double>& getValues() const                    { return values; }
};

#endif // PCHIP_H
