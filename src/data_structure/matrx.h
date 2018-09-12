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

#include<cstdint>
#include<cassert>

template<typename T>
class Matrix
{
	std::size_t sizeX;
	std::size_t sizeY;
	T* field = nullptr;
public:
	typedef T value_type;

	Matrix() = default;
	Matrix(std::size_t x, std::size_t y)
	: sizeX(x)
	, sizeY(y)
	, field(new T[x*y])
	{
	}

	~Matrix()
	{
		delete[] field;
	}


	T* begin()                                                      { return field; }
	T* end()                                                        { return field+sizeX*sizeY; }
	const T* begin()                                          const { return field; }
	const T* end()                                            const { return field+sizeX*sizeY; }

	T* scanLine(std::size_t y)                                      { assert(y < sizeY); return field+sizeX*y; }
	const T* scanLine(std::size_t y)                          const { assert(y < sizeY); return field+sizeX*y; }

	Matrix(const Matrix& other)            = delete;
	Matrix& operator=(const Matrix& other) = delete;


	T& operator()(std::size_t x, std::size_t y)                     { assert(y < sizeY); assert(x < sizeX); return field[x + y*sizeX]; }
	const T& operator()(std::size_t x, std::size_t y)         const { assert(y < sizeY); assert(x < sizeX); return field[x + y*sizeX]; }
	std::size_t getSizeX()                                    const { return sizeX; }
	std::size_t getSizeY()                                    const { return sizeY; }

	void resize(std::size_t x, std::size_t y)
	{
		T* fieldOld = field;
		field = new T[x*y];
		delete[] fieldOld;
		sizeX = x;
		sizeY = y;
	}
};
