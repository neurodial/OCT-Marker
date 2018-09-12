/*
 * Copyright (c) 2018 Kay Gawlik <kaydev@amarunet.de> <kay.gawlik@beuth-hochschule.de> <kay.gawlik@charite.de>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#pragma once

#include<QObject>
#include<QString>
#include<vector>

#include<octdata/datastruct/segmentationlines.h>

class ThicknessmapTemplates : QObject
{
	Q_OBJECT
public:
	enum class UseColorMap { hsv, yellow };
	class Configuration
	{
		const QString name;
		const OctData::Segmentationlines::SegmentlineType line1;
		const OctData::Segmentationlines::SegmentlineType line2;
		const double minValue;
		const double maxValue;
		const UseColorMap useColorMap;
	public:
		Configuration(const QString& name
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


		const QString& getName()                                 const { return name; }
		OctData::Segmentationlines::SegmentlineType getLine1()   const { return line1; }
		OctData::Segmentationlines::SegmentlineType getLine2()   const { return line2; }
		double getMinValue()                                     const { return minValue; }
		double getMaxValue()                                     const { return maxValue; }
		UseColorMap getUseColorMap()                             const { return useColorMap; }
	};

	static ThicknessmapTemplates& getInstance() { static ThicknessmapTemplates instance; return instance; }

	const std::vector<Configuration>& getConfigurations()        const { return configurations; }

private:
	ThicknessmapTemplates();

	std::vector<Configuration> configurations;

};
