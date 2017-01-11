#ifndef SIMPLEMATCOMPRESS_H
#define SIMPLEMATCOMPRESS_H

#include <vector>
#include <cstdint>

#include <boost/serialization/vector.hpp>


namespace cv { class Mat; }

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
	void readMat(const cv::Mat& mat);

	void writeMat(cv::Mat& mat) const;

	bool isEmpty(uint8_t defaultValue) const;
};


#endif // SIMPLEMATCOMPRESS_H
