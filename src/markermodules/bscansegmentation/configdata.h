#pragma once

#include <cstddef>
#include <cstdint>

namespace BScanSegmentationMarker
{
		typedef uint8_t internalMatType;

		static const internalMatType paintArea0Value = 0;
		static const internalMatType paintArea1Value = 1;

		static const internalMatType markermatInitialValue = paintArea0Value;


	struct ThresholdData
	{
		enum class Direction { left, right, up, down };
		enum class Method { Absolute, Relative };

		Direction   direction     = Direction::down;
		Method      method        = Method::Absolute;
		std::size_t neededStrikes = 10;
		int         absoluteValue = 70;
		double      relativeFrac  = 0.4;
	};

	enum class LocalMethod { Threshold, Paint, Operation };
	enum class Operation   { Erode, Dilate };

	struct PaintData
	{
		enum class PaintMethod { Circle, Rect };
		enum class PaintColor  { Area0, Auto, Area1 };

		PaintMethod paintMethod;
		PaintColor  paintColor;
	};

	/*
	struct LocalData
	{

		Method method;

		// Threshold
		ThresholdData thresholdData;

		// Paint

		Operation operation;
	};
	*/
}
