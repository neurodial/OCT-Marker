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
