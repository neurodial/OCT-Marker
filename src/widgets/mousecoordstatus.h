#ifndef MOUSECOORDSTATUS_H
#define MOUSECOORDSTATUS_H

#include<QLabel>

class BScanMarkerWidget;

class MouseCoordStatus : public QLabel
{
	Q_OBJECT

	BScanMarkerWidget* bscanWidget;

public:
	MouseCoordStatus(BScanMarkerWidget* bscanWidget);
	virtual ~MouseCoordStatus();


private slots:

	void mousePosInImage(int x, int y);
	void mouseLeaveImage();

};

#endif // MOUSECOORDSTATUS_H
