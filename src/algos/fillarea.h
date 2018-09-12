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

#include<vector>


template<typename Painter>
class FillArea
{
	struct Element
	{
		Element(std::size_t x, std::size_t y) : x(x), y(y) {}

		std::size_t x;
		std::size_t y;
	};
// 	const std::size_t sizeX;
// 	const std::size_t sizeY;

	Painter& painter;

	std::vector<Element> elementList;

	void checkAndAddElement(const Element& ele)
	{
		if(painter.isInArea(ele.x, ele.y))
		{
			painter.paint(ele.x, ele.y);
			elementList.push_back(ele);
		}
	}

	void handleElement(Element& ele)
	{
		if(ele.x > 0)
			checkAndAddElement(Element(ele.x-1, ele.y  ));
		if(ele.y > 0)
			checkAndAddElement(Element(ele.x  , ele.y-1));
// 		if(ele.x < sizeX-1)
			checkAndAddElement(Element(ele.x+1, ele.y  ));
// 		if(ele.y < sizeY-1)
			checkAndAddElement(Element(ele.x  , ele.y+1));

	}

	void handleList()
	{
		while(elementList.size() > 0)
		{
			Element actElement = elementList.back();
			elementList.pop_back();

			handleElement(actElement);
		}
	}

public:
	FillArea(Painter& p) // , std::size_t sizeX, std::size_t sizeY)
	: painter(p)
// 	, sizeX(sizeX)
// 	, sizeY(sizeY)
	{
	}

	void fill(std::size_t x, std::size_t y)
	{
		checkAndAddElement(Element(x, y));
		handleList();
	}

};
