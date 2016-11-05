#pragma once

#include <cstddef>
#include <cstdint>

namespace BScanSegmentationMarker
{
		typedef uint8_t internalMatType;

		static const internalMatType paintArea0Value = 0;
		static const internalMatType paintArea1Value = 1;

		static const internalMatType markermatInitialValue = paintArea0Value;


	class ThresholdData
	{
	public:
		enum class Direction { left, right, up, down };
		enum class Method { Absolute, Relative };

		Direction   direction     = Direction::down;
		Method      method        = Method::Relative;
		std::size_t neededStrikes = 10;
		int         absoluteValue = 80;
		double      relativeFrac  = 0.5;

		bool operator==(const ThresholdData& other) const
		{
			return direction     == other.direction
			    && method        == other.method
			    && neededStrikes == other.neededStrikes
			    && absoluteValue == other.absoluteValue
			    && relativeFrac  == other.relativeFrac ;
		}
	};

	enum class LocalMethod { Threshold, Paint, Operation };
	enum class Operation   { Erode, Dilate, OpenClose, Median };

	struct PaintData
	{
		enum class PaintMethod { Circle, Rect, Pen };
		enum class PaintColor  { Area0, Auto, Area1 };

		PaintMethod paintMethod = PaintMethod::Circle;
		PaintColor  paintColor  = PaintColor::Auto;

		bool operator==(const PaintData& other) const
		{
			return paintMethod == other.paintMethod
			    && paintColor  == other.paintColor;
		}
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
