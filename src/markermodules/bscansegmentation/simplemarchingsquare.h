#ifndef SIMPLEMARCHINGSQUARE_H
#define SIMPLEMARCHINGSQUARE_H


#include<cstdint>

class PaintSegLine;

class SimpleMarchingSquare
{
public:
	void handleSquare(uint8_t gridVal0, uint8_t gridVal1, uint8_t gridVal2, uint8_t gridVal3, int i, int j, PaintSegLine& painter);
};

#endif // SIMPLEMARCHINGSQUARE_H
