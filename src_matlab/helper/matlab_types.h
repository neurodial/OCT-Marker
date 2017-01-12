#pragma once

#include <stdint.h>
#include "mex.h"

template<typename T>
struct MatlabType {};

template<>
struct MatlabType<bool>
{
	constexpr static const mxClassID classID = mxLOGICAL_CLASS;
};

template<>
struct MatlabType<int8_t>
{
	constexpr static const mxClassID classID = mxINT8_CLASS;
};

template<>
struct MatlabType<uint8_t>
{
	constexpr static const mxClassID classID = mxUINT8_CLASS;
};


template<>
struct MatlabType<int16_t>
{
	constexpr static const mxClassID classID = mxINT16_CLASS;
};

template<>
struct MatlabType<uint16_t>
{
	constexpr static const mxClassID classID = mxUINT16_CLASS;
};


template<>
struct MatlabType<uint32_t>
{
	constexpr static const mxClassID classID = mxUINT32_CLASS;
};

template<>
struct MatlabType<int32_t>
{
	constexpr static const mxClassID classID = mxINT32_CLASS;
};


template<>
struct MatlabType<int64_t>
{
	constexpr static const mxClassID classID = mxINT64_CLASS;
};

template<>
struct MatlabType<uint64_t>
{
	constexpr static const mxClassID classID = mxUINT64_CLASS;
};


template<>
struct MatlabType<float>
{
	constexpr static const mxClassID classID = mxSINGLE_CLASS;
};

template<>
struct MatlabType<double>
{
	constexpr static const mxClassID classID = mxDOUBLE_CLASS;
};


