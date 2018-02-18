#pragma once

#include<cstdint>

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

	T* scanLine(std::size_t y)                                      { return field+sizeX*y; }

	Matrix(const Matrix& other)            = delete;
	Matrix& operator=(const Matrix& other) = delete;


	T& operator()(std::size_t x, std::size_t y)                     { return field[x + y*sizeX]; }
	const T& operator()(std::size_t x, std::size_t y)        const  { return field[x + y*sizeX]; }
	std::size_t getSizeX()                                   const  { return sizeX; }
	std::size_t getSizeY()                                   const  { return sizeY; }

	void resize(std::size_t x, std::size_t y)
	{
		T* fieldOld = field;
		field = new T[x*y];
		delete[] fieldOld;
		sizeX = x;
		sizeY = y;
	}
};
