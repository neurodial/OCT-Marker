#pragma once

#include<string>
#include<vector>

#include<octdata/datastruct/segmentationlines.h>

class ThicknessmapTemplates
{
public:
	enum class UseColorMap { hsv, yellow };
	class Configuration
	{
		std::string name;
		OctData::Segmentationlines::SegmentlineType line1;
		OctData::Segmentationlines::SegmentlineType line2;
		double minValue;
		double maxValue;
		UseColorMap useColorMap;
	public:
		Configuration(const std::string& name
		            , OctData::Segmentationlines::SegmentlineType line1
		            , OctData::Segmentationlines::SegmentlineType line2
		            , double minValue
		            , double maxValue
		            , UseColorMap useColorMap)
		: name       (name       )
		, line1      (line1      )
		, line2      (line2      )
		, minValue   (minValue   )
		, maxValue   (maxValue   )
		, useColorMap(useColorMap)
		{}


		const std::string& getName()                             const { return name; }
		OctData::Segmentationlines::SegmentlineType getLine1()   const { return line1; }
		OctData::Segmentationlines::SegmentlineType getLine2()   const { return line2; }
		double getMinValue()                                     const { return minValue; }
		double getMaxValue()                                     const { return maxValue; }
		UseColorMap getUseColorMap()                             const { return useColorMap; }
	};

	static ThicknessmapTemplates& getInstance() { static ThicknessmapTemplates instance; return instance; }

private:
	ThicknessmapTemplates();

	std::vector<Configuration> configurations;

};
