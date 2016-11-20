#ifndef SCROLLAREAPAN_H
#define SCROLLAREAPAN_H

#include <QScrollArea>

class QInputEvent;

class ScrollAreaPan : public QScrollArea
{
	enum class PanStatus { None, PanReady, Paning };
	enum class MausButton { Undef, Pressed, Unpressed };

public:
	explicit ScrollAreaPan(QWidget *parent = Q_NULLPTR) : QScrollArea(parent) {}
protected:
	void mousePressEvent  (QMouseEvent* e) override;
	void mouseMoveEvent   (QMouseEvent* e) override;
	void mouseReleaseEvent(QMouseEvent* e) override;
	void keyPressEvent    (QKeyEvent*   e) override;
	void keyReleaseEvent  (QKeyEvent*   e) override;
private:
	QPoint mousePos;
	PanStatus panStatus = PanStatus::None;

	bool upadtePanStatus(bool modifierPressed, MausButton mouseButtonPressed);
};

#endif // SCROLLAREAPAN_H
