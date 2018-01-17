#pragma once

#include<cstdint>

template<typename T>
class Matrix
{
	std::size_t sizeX;
	std::size_t sizeY;
	T* field = nullptr;
public:
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




	Matrix(const Matrix& other)            = delete;
	Matrix& operator=(const Matrix& other) = delete;


	T& operator()(std::size_t x, std::size_t y)                     { return field[x + y*sizeX]; }
	std::size_t getSizeX()                                          { return sizeX; }
	std::size_t getSizeY()                                          { return sizeY; }
};
