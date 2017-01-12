#pragma once


#include "matlab_types.h"
#include "mex.h"

#include <vector>

#include <tuple>


template<typename T>
void createMatlabVector(const std::vector<T>& vec, mxArray*& matlabMat)
{
	if(matlabMat)
		return;

	std::size_t size = vec.size();
	matlabMat = mxCreateNumericMatrix(size, 1, MatlabType<T>::classID, mxREAL);

	if(!matlabMat)
		return;

	T* matlabPtr = reinterpret_cast<T*>(mxGetPr(matlabMat));
	for(T value : vec)
	{
		*matlabPtr = value;
		++matlabPtr;
	}
}

template<typename T>
void createMatlabArray(const T* vec, mxArray*& matlabMat, std::size_t size)
{
	if(matlabMat)
		return;

	matlabMat = mxCreateNumericMatrix(size, 1, MatlabType<T>::classID, mxREAL);

	if(!matlabMat)
		return;

	T* matlabPtr = reinterpret_cast<T*>(mxGetPr(matlabMat));
	const T* vecEnd = vec + size;
	while(vec != vecEnd)
	{
		*matlabPtr = *vec;
		++matlabPtr;
		++vec;
	}
}

template<typename T>
mxArray* createMatlabArray(const T* vec, std::size_t size)
{
	mxArray* matlabMat = nullptr;
	createMatlabArray<T>(vec, matlabMat, size);
	return matlabMat;
}


template<typename T>
void copyVec2MatlabRow(const std::vector<T>& vec, std::size_t row, mxArray* matlabMat)
{
	if(!matlabMat)
	{
		mexPrintf("matlabMat == nullptr\n");
		return;
	}

	if(mxGetClassID(matlabMat) != MatlabType<T>::classID)
	{
		mexPrintf("Wrong ClassID: %d != %d\n", mxGetClassID(matlabMat), MatlabType<T>::classID);
		return;
	}

	T* matlabPtr = reinterpret_cast<T*>(mxGetPr(matlabMat));
	std::size_t cols = mxGetN(matlabMat);
	std::size_t rows = mxGetM(matlabMat);

	if(cols < vec.size())
	{
		mexPrintf("Matrix zu klein:: %d < %d\n", cols, vec.size());
		return;
	}

	matlabPtr += row;
	for(T value : vec)
	{
		*matlabPtr = value;
		matlabPtr += rows;
	}
}

template<typename T>
void copyVec2MatlabCol(const std::vector<T>& vec, std::size_t col, mxArray* matlabMat)
{
	if(!matlabMat)
	{
		mexPrintf("matlabMat == nullptr\n");
		return;
	}

	if(mxGetClassID(matlabMat) != MatlabType<T>::classID)
	{
		mexPrintf("Wrong ClassID: %d != %d\n", mxGetClassID(matlabMat), MatlabType<T>::classID);
		return;
	}

	T* matlabPtr = reinterpret_cast<T*>(mxGetPr(matlabMat));
	std::size_t cols = mxGetN(matlabMat);
	std::size_t rows = mxGetM(matlabMat);

	if(rows < vec.size())
	{
		mexPrintf("Matrix zu klein:: %d < %d\n", cols, vec.size());
		return;
	}

	matlabPtr += col*rows;
	for(T value : vec)
	{
		*matlabPtr = value;
		++matlabPtr;
	}
}

inline std::tuple<int, int> getMatrixSize(const mxArray* matlabMat)
{
	return std::make_tuple(mxGetN(matlabMat), mxGetM(matlabMat));
}




template<typename T>
T getValueConvert(const mxArray* const matlabMat, const double* ptr)
{
	if(!matlabMat)
		return T();

	switch(mxGetClassID(matlabMat))
	{
#define HANDLE_TYPE(TYPE) case MatlabType<TYPE>::classID: return static_cast<T>(*(reinterpret_cast<const TYPE*>(ptr)));
		HANDLE_TYPE(double);
		HANDLE_TYPE(float);
		HANDLE_TYPE(bool);
		HANDLE_TYPE(uint8_t);
		HANDLE_TYPE(uint16_t);
		HANDLE_TYPE(uint32_t);
		HANDLE_TYPE(uint64_t);
		HANDLE_TYPE( int8_t);
		HANDLE_TYPE( int16_t);
		HANDLE_TYPE( int32_t);
		HANDLE_TYPE( int64_t);
#undef HANDLE_TYPE
		default:
			mexPrintf("unhandled Type: %d", mxGetClassID(matlabMat));
	}

	return T();
}

template<typename T>
T getScalarConvert(const mxArray* matlabMat)
{
	if(!matlabMat)
		return T();

	switch(mxGetClassID(matlabMat))
	{
#define HANDLE_TYPE(TYPE) case MatlabType<TYPE>::classID: return static_cast<T>(*(reinterpret_cast<TYPE*>(mxGetPr(matlabMat))));
		HANDLE_TYPE(double);
		HANDLE_TYPE(float);
		HANDLE_TYPE(bool);
		HANDLE_TYPE(uint8_t);
		HANDLE_TYPE(uint16_t);
		HANDLE_TYPE(uint32_t);
		HANDLE_TYPE(uint64_t);
		HANDLE_TYPE( int8_t);
		HANDLE_TYPE( int16_t);
		HANDLE_TYPE( int32_t);
		HANDLE_TYPE( int64_t);
#undef HANDLE_TYPE
		default:
			mexPrintf("unhandled Type: %d", mxGetClassID(matlabMat));
	}

	return T();
}

template<typename T>
inline T getConfigFromStruct(const mxArray* const mxConfig, const char* name, const T defaultValue)
{
	mxArray* mxOpt = mxGetField(mxConfig, 0, name);
	if(mxOpt)
		return getScalarConvert<T>(mxOpt);

	return defaultValue;
}

