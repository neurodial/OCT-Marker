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
	bool isEmpty(uint8_t defaultValue) const;

	int getRows() const { return rows; }
	int getCols() const { return cols; }

	bool readFromMat(const uint8_t* mat, int rows, int cols);
	bool writeToMat (      uint8_t* mat, int rows, int cols) const;

	bool isEqual(const uint8_t* mat, int rows, int cols) const;
	bool operator==(const SimpleMatCompress& other) const;
};


#endif // SIMPLEMATCOMPRESS_H
