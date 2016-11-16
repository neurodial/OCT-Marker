#pragma once

#include <cstddef>
#include <cstdint>

namespace BScanSegmentationMarker
{
	typedef uint8_t internalMatType;

	static const internalMatType paintArea0Value = 0;
	static const internalMatType paintArea1Value = 1;

	static const internalMatType markermatInitialValue = paintArea0Value;

	enum class ThresholdMethod { Absolute, Relative };

	class ThresholdDirectionData
	{
	public:
		enum class Direction { left, right, up, down };

		Direction       direction        = Direction::down;
		ThresholdMethod method           = ThresholdMethod::Relative;
		int             neededStrikes    = 6;
		double          negStrikesFactor = 0.6;
		internalMatType absoluteValue    = 80;
		double          relativeFrac     = 0.5;

		bool operator==(const ThresholdDirectionData& other) const
		{
			return direction     == other.direction
			    && method        == other.method
			    && neededStrikes == other.neededStrikes
			    && absoluteValue == other.absoluteValue
			    && relativeFrac  == other.relativeFrac ;
		}
	};

	class ThresholdData
	{
	public:
		ThresholdMethod method           = ThresholdMethod::Relative;
		internalMatType absoluteValue    = 80;
		double          relativeFrac     = 0.5;

		bool operator==(const ThresholdData& other) const
		{
			return absoluteValue == other.absoluteValue
			    && relativeFrac  == other.relativeFrac;
		}
	};

	enum class LocalMethod { None, ThresholdDirection, Threshold, Paint, Operation };
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
