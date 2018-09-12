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

#ifndef SIMPLEMATCOMPRESS_H
#define SIMPLEMATCOMPRESS_H

#include <vector>
#include <cstdint>

#include <boost/serialization/vector.hpp>

class SimpleMatCompress
{
	friend class boost::serialization::access;
	struct MatSegment
	{
		friend class boost::serialization::access;

		MatSegment() {}

		MatSegment(int length, uint8_t value)
		: length(length)
		, value (value )
		{ }

		int    length = 0;
		uint8_t value = 0;

		template<class Archive>
		void serialize(Archive & ar, const unsigned int /*version*/)
		{
			ar & length;
			ar & value;
		}

		bool operator==(const MatSegment& other) const
		{
			return length == other.length
			    && value  == other.value ;
		}
	};

	std::vector<MatSegment> segmentsChange;
	int rows = 0;
	int cols = 0;
	int sumSegments = 0;

	template<class Archive>
	void serialize(Archive & ar, const unsigned int /*version*/)
	{
		ar & rows;
		ar & cols;
		ar & segmentsChange;
	}
	void addSegment(int length, uint8_t value);

public:
	SimpleMatCompress() = default;
	SimpleMatCompress(int rows, int cols, uint8_t initValue);

	bool isEmpty(uint8_t defaultValue) const;

	int getRows() const { return rows; }
	int getCols() const { return cols; }

	bool readFromMat(const uint8_t* mat, int rows, int cols);
	bool writeToMat (      uint8_t* mat, int rows, int cols) const;

	bool isEqual(const uint8_t* mat, int rows, int cols) const;
	bool operator==(const SimpleMatCompress& other) const;
};


#endif // SIMPLEMATCOMPRESS_H
