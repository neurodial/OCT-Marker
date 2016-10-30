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

}
