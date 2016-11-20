#ifndef SCROLLAREAPAN_H
#define SCROLLAREAPAN_H

#include <QScrollArea>

class ScrollAreaPan : public QScrollArea
{
public:
	explicit ScrollAreaPan(QWidget *parent = Q_NULLPTR) : QScrollArea(parent) {}
protected:
	void mousePressEvent  (QMouseEvent* e) override;
	void mouseMoveEvent   (QMouseEvent* e) override;
	void mouseReleaseEvent(QMouseEvent* e) override;
private:
	QPoint mousePos;
	bool paning = false;

	void setPan(bool pan);
};

#endif // SCROLLAREAPAN_H
