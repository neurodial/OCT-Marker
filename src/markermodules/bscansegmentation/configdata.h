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

	enum class LocalMethod { None, ThresholdDirection, Threshold, Paint, Operation, NN };
	enum class Operation   { Erode, Dilate, OpenClose, Median };

	struct PaintData
	{
		enum class PaintMethod { Circle, Rect, Pen };

		PaintMethod paintMethod = PaintMethod::Circle;

		bool operator==(const PaintData& other) const
		{
			return paintMethod == other.paintMethod;
		}
	};

	struct ColorData
	{
		enum class PaintColor  { Area0, Auto, Area1 };
		PaintColor  paintColor = PaintColor::Auto;

		bool operator==(const ColorData& other) const
		{
			return paintColor == other.paintColor;
		}
	};

	struct NNTrainData
	{
		enum class TrainMethod { RPROP, Backpropergation};

		double      epsilon       =   0.000001;
		int         maxIterations = 200;
		TrainMethod trainMethod   = TrainMethod::Backpropergation;

		double learnRate   = 0.005;
		double momentScale = 0.05;

		double delta0      = 0.1;
		double nuePlus     = 0.9;
		double nueMinus    = 1.1;
		double deltaMin    = 0.01;
		double deltaMax    = 5.0;
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
