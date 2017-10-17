#pragma once

#include<vector>
#include<data_structure/point2d.h>
#include<data_structure/conture2d.h>
#include<data_structure/conturesegment.h>



class OrderdContures2D
{
	const std::vector<Line2D  >& lines;
	const std::vector<LineNode>& points;
	std::vector<bool> handeldPoints;
	std::vector<bool> handeldLines;

	std::vector<ContureSegment> segments;

	std::size_t addLineToSegment(ContureSegment& segment, std::size_t lineIdx  , std::size_t nodeIdx);
	void testAndAddCreateSegment(std::size_t lineIndex, std::size_t nodeIdx);

	void handelLineSegment(const std::size_t nodeId);

	void handleOpenEnds();
	void handleOthers();
public:
	OrderdContures2D(const Conture2D& conture);


	const std::vector<ContureSegment>& getSegments()          const { return segments; }

};

