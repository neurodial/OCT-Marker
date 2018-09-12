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

#ifndef SLOBSCANDISTANCEMAP_H
#define SLOBSCANDISTANCEMAP_H

#include<data_structure/matrx.h>
#include<limits>
#include<vector>

#include "point2d.h"


namespace OctData { class Series; }

class SloBScanDistanceMap
{
public:
	class InfoBScanDist
	{
	public:
		double      distance = std::numeric_limits<double>::infinity();
		std::size_t bscan    = std::numeric_limits<std::size_t>::max();
		std::size_t ascan    = std::numeric_limits<std::size_t>::max();
	};
	class PixelInfo
	{
	public:
		InfoBScanDist bscan1;
		InfoBScanDist bscan2;
		bool init = false;
	};

	typedef Matrix<PixelInfo> PreCalcDataMatrix;


	SloBScanDistanceMap();
	~SloBScanDistanceMap();

	void createData(const OctData::Series* series);


	const PreCalcDataMatrix* getDataMatrix() const { return preCalcDataMatrix; }

private:
	PreCalcDataMatrix* preCalcDataMatrix = nullptr;

};

#endif // SLOBSCANDISTANCEMAP_H
