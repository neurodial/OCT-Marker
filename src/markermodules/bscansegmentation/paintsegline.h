#pragma once


#include<data_structure/point2d.h>

#include<QPainter>


class PaintSegLine
{
public:
	virtual void paintLine(const Point2D& p1, const Point2D& p2) = 0;
};


class PaintFactor : public PaintSegLine
{
protected:
	QPainter& painter;
public:
	PaintFactor(QPainter& painter) : painter(painter) {}

	void setPen(QPen& p) { painter.setPen(p); }
};

class PaintFactor1 : public PaintFactor
{
public:
	PaintFactor1(QPainter& painter) : PaintFactor(painter) {}

	void paintLine(const Point2D& p1, const Point2D& p2)
	{
		painter.drawPoint(p1.getX(), p1.getY());
		painter.drawPoint(p2.getX(), p2.getY());
	}
};

class PaintFactorN : public PaintFactor
{
	double factor = 1;
public:
	PaintFactorN(QPainter& painter, double factor) : PaintFactor(painter), factor(factor) {}


	void paintLine(const Point2D& p1, const Point2D& p2)
	{
		painter.drawLine(static_cast<int>((p1.getX())*factor + 0.5)
		               , static_cast<int>((p1.getY())*factor + 0.5)
		               , static_cast<int>((p2.getX())*factor + 0.5)
		               , static_cast<int>((p2.getY())*factor + 0.5));
	}

// 	inline void paint(const uint8_t* p00, const uint8_t* p10, const uint8_t* p01, int w, int h, uint8_t mask, )
// 	{
// 		if((*p00 & mask) != (*p10 & mask)) painter.drawLine(static_cast<int>((w+1)*factor + 0.5), static_cast<int>((h  )*factor + 0.5), static_cast<int>((w+1)*factor + 0.5), static_cast<int>((h+1)*factor + 0.5));
// 		if((*p00 & mask) != (*p01 & mask)) painter.drawLine(static_cast<int>((w  )*factor + 0.5), static_cast<int>((h+1)*factor + 0.5), static_cast<int>((w+1)*factor + 0.5), static_cast<int>((h+1)*factor + 0.5));
// 	}
};
