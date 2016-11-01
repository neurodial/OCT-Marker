#pragma once

#include <cstddef>

namespace BScanSegmentationMarker
{

	struct ThresholdData
	{
		enum class Direction { left, right, up, down };
		enum class Method { Absolute, Relative };

		Direction   direction     = Direction::down;
		Method      method        = Method::Relative;
		std::size_t neededStrikes = 1;
		int         absoluteValue = 80;
		double      relativeFrac  = 45;
	};

	struct PaintData
	{
		enum class PaintMethod { Circle, Rect };
		enum class PaintColor  { Area0, Auto, Area1 };

		PaintMethod paintMethod;
		PaintColor  paintColor;
		int paintSize = 10;
	};

	struct LocalData
	{
		enum class Method      { Threshold, Paint, Operation };
		enum class Operation   { Erode, Dilate };

		Method method;

		// Threshold
		ThresholdData thresholdData;

		// Paint

		Operation operation;
	};
}
