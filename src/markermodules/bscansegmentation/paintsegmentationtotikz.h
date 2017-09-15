#ifndef PAINTSEGMENTATIONTOTIKZ_H
#define PAINTSEGMENTATIONTOTIKZ_H

#include<vector>
#include<QString>

#include<data_structure/conture2d.h>

#include"paintsegline.h"

class QPen;

class PaintSegmentationToTikz : public PaintSegLine
{
	int width;
	int height;

	std::vector<int> pointIndexMap;
	Conture2D conture;

	std::size_t getPointIndex(const Point2D& p)
	{
		int x = static_cast<int>(p.getX());
		int y = static_cast<int>(p.getY());

		if(x<0 || x>width || y<0 || y>height)
			throw("invalid index");

		std::size_t index = x + y*width;

		if(pointIndexMap[index] == -1)
			pointIndexMap[index] = conture.addPoint(p);

		return pointIndexMap[index];
	}


public:
	PaintSegmentationToTikz(int width, int height)
	:width (width )
	,height(height)
	{
		pointIndexMap.assign(static_cast<std::size_t>(height*height), -1);
	}


	void setPen(QPen&) {}

	void paintLine(const Point2D& p1, const Point2D& p2) override
	{
		try
		{
			std::size_t pIndex1 = getPointIndex(p1);
			std::size_t pIndex2 = getPointIndex(p2);

			conture.addLine(pIndex1, pIndex2);
		}
		catch(const char*)
		{}
	}


	const QString getTikzCode() const;
};


#endif // PAINTSEGMENTATIONTOTIKZ_H
