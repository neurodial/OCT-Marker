#pragma once


namespace BScanSegmentationMarker
{

	struct ThresholdData
	{
		enum class Direction { left, right, up, down };
		enum class Method { Absolute, Relative };

		int    neededStrikes = 1;
		int    absoluteValue = 80;
		double relativeFrac  = 45;
	};

	struct LocalData
	{
		enum class Method      { Threshold, Paint, Operation };
		enum class PaintMethod { Circle, Rect };
		enum class PaintColor  { Area0, Auto, Area1 };
		enum class Operation   { Erode, Dilate };

		Method method;

		// Threshold
		ThresholdData thresholdData;

		// Paint
		PaintMethod paintMethod;
		PaintColor  paintColor;
		int paintSize = 10;

		Operation operation;
	};
}
