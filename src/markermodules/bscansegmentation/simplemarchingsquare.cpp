#include "simplemarchingsquare.h"


#include<data_structure/point2d.h>
#include"paintsegline.h"

namespace
{
	constexpr const int edgeTable[16]={
	0,      // 0000
	1 | 8,  // 0001
	1 | 2,  // 0010
	2 | 8,  // 0011
	2 | 4,  // 0100
	15   ,  // 0101 ??
	1 | 4,  // 0110
	4 | 8,  // 0111
	4 | 8,  // 1000
	1 | 4,  // 1001
	15   ,  // 1010 ??
	2 | 4,  // 1011
	2 | 8,  // 1100
	1 | 2,  // 1101
	1 | 8,  // 1110
	0       // 1111
	};

/*
    (3) --4,2-- (2)
     |           |
    8,3         2,1
     |           |
    (0) --1,0-- (1)
*/

	constexpr const int triTable[16][5] =
	{{-1, -1, -1, -1, -1},    // 0000
	 { 0,  3, -1, -1, -1},    // 0001
	 { 1,  0, -1, -1, -1},    // 0010
	 { 1,  3, -1, -1, -1},    // 0011
	 { 2,  1, -1, -1, -1},    // 0100
	 { 0,  2,  1,  3, -1},    // 0101 ??
	 { 2,  0, -1, -1, -1},    // 0110
	 { 2,  3, -1, -1, -1},    // 0111
	 { 3,  2, -1, -1, -1},    // 1000
	 { 0,  2, -1, -1, -1},    // 1001
	 { 0,  2,  1,  3, -1},    // 1010 ??
	 { 1,  2, -1, -1, -1},    // 1011
	 { 3,  1, -1, -1, -1},    // 1100
	 { 0,  1, -1, -1, -1},    // 1101
	 { 3,  0, -1, -1, -1},    // 1110
	 {-1, -1, -1, -1, -1}     // 1111
	};

	Point2D vertexInterp(const Point2DInt& p1, const Point2DInt& p2, uint8_t valp1, uint8_t valp2)
	{
		if(valp1 == valp2)
			return Point2D(p1.getX() + 0.5, p1.getY() + 0.5);

		return Point2D((p1.getX() + p2.getX())/2. + 0.5, (p1.getY() + p2.getY())/2. + 0.5);
	}

}


void SimpleMarchingSquare::handleSquare(uint8_t gridVal0, uint8_t gridVal1, uint8_t gridVal2, uint8_t gridVal3, int i, int j, PaintSegLine& painter)
{
	Point2D vertlist[4];

	int cubeindex = 0;
	if(gridVal0 != 0) cubeindex |= 1;
	if(gridVal1 != 0) cubeindex |= 2;
	if(gridVal2 != 0) cubeindex |= 4;
	if(gridVal3 != 0) cubeindex |= 8;

	const int cubeType = edgeTable[cubeindex];

	 // Cube is entirely in/out of the surface
	if(cubeType == 0)
		return;

	const Point2DInt gridPoint0(j-1, i  );
	const Point2DInt gridPoint1(j  , i  );
	const Point2DInt gridPoint2(j  , i-1);
	const Point2DInt gridPoint3(j-1, i-1);


	// Find the vertices where the surface intersects the cube
	if(cubeType & 1) vertlist[0] = vertexInterp(gridPoint0, gridPoint1, gridVal0, gridVal1);
	if(cubeType & 2) vertlist[1] = vertexInterp(gridPoint1, gridPoint2, gridVal1, gridVal2);
	if(cubeType & 4) vertlist[2] = vertexInterp(gridPoint2, gridPoint3, gridVal2, gridVal3);
	if(cubeType & 8) vertlist[3] = vertexInterp(gridPoint3, gridPoint0, gridVal3, gridVal0);

	const int* const cubeTriLUT = triTable[cubeindex];
	for(int i=0; i<4; i+=2)
		if(cubeTriLUT[i] != -1)
			if(vertlist[cubeTriLUT[i+0]] != vertlist[cubeTriLUT[i+1]])
				painter.paintLine(vertlist[cubeTriLUT[i+0]], vertlist[cubeTriLUT[i+1]]);
}
