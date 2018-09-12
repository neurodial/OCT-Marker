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

#include<cmath>


// https://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm
template<typename Painter>
class LineBresenhamAlgo
{
	Painter& painter;

	void plotLineLow(int x0, int y0, int x1, int y1)
	{
		int dx = x1 - x0;
		int dy = y1 - y0;
		int yi = 1;
		if(dy < 0)
		{
			yi = -1;
			dy = -dy;
		}
		int D = 2*dy - dx;
		int y = y0;

		for(int x = x0; x <= x1; ++x)
		{
			painter.plot(x, y);
			if(D > 0)
			{
				 y = y + yi;
				 D = D - 2*dx;
			}
			D = D + 2*dy;
		}
	}


	void plotLineHigh(int x0, int y0, int x1, int y1)
	{
		int dx = x1 - x0;
		int dy = y1 - y0;
		int xi = 1      ;
		if(dx < 0)
		{
			xi = -1 ;
			dx = -dx;
		}
		int D = 2*dx - dy;
		int x = x0;

		for(int y = y0; y < y1; ++y)
		{
			painter.plot(x,y);
			if(D > 0)
			{
				 x = x + xi;
				 D = D - 2*dy;
			}
			D = D + 2*dx;
		}
	}

public:
	LineBresenhamAlgo(Painter& painter) : painter(painter) {}

	void plotLine(int x0, int y0, int x1, int y1)
	{
		if(std::abs(y1 - y0) < std::abs(x1 - x0))
		{
			if(x0 > x1)
				plotLineLow(x1, y1, x0, y0);
			else
				plotLineLow(x0, y0, x1, y1);
		}
		else
		{
			if(y0 > y1)
				plotLineHigh(x1, y1, x0, y0);
			else
				plotLineHigh(x0, y0, x1, y1);
		}
	}

};
