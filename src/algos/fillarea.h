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
