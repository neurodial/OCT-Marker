#ifndef PAINTSEGMENTATIONTOTIKZ_H
#define PAINTSEGMENTATIONTOTIKZ_H

#include<vector>
#include<QString>

#include<data_structure/conture2d.h>

class QPen;

class PaintSegmentationToTikz
{
	int width;
	int height;

	std::vector<int> pointIndexMap;
	Conture2D conture;

	std::size_t getPointIndex(int x, int y)
	{
		if(x<0 || x>width || y<0 || y>height)
			return -1; // TODO
		std::size_t index = x + y*width;

		if(pointIndexMap[index] == -1)
			pointIndexMap[index] = conture.addPoint(Point2D(x, y));

		return pointIndexMap[index];
	}

	void drawLine(int x1, int y1, int x2, int y2)
	{
		std::size_t pIndex1 = getPointIndex(x1, y1);
		std::size_t pIndex2 = getPointIndex(x2, y2);

		conture.addLine(pIndex1, pIndex2);
	}

public:
	PaintSegmentationToTikz(int width, int height)
	:width (width )
	,height(height)
	{
		pointIndexMap.assign(height*height, -1);
	}

	void setPen(QPen&) {}

	inline void paint(const uint8_t* p00, const uint8_t* p10, const uint8_t* p01, int w, int h, uint8_t mask, double /*factor*/)
	{
		if((*p00 & mask) != (*p10 & mask)) drawLine(w+1, h  , w+1, h+1);
		if((*p00 & mask) != (*p01 & mask)) drawLine(w  , h+1, w+1, h+1);
	}

	const QString getTikzCode() const;
};


#endif // PAINTSEGMENTATIONTOTIKZ_H
