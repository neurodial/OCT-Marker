#ifndef SCROLLAREAPAN_H
#define SCROLLAREAPAN_H

#include <QScrollArea>

class QInputEvent;

class ScrollAreaPan : public QScrollArea
{
	Q_OBJECT

	enum class PanStatus { None, PanReady, Paning };
	enum class MausButton { Undef, Pressed, Unpressed };

public:
	explicit ScrollAreaPan(QWidget *parent = Q_NULLPTR);

	int getVScrollbarWidth () const;
	int getHScrollbarHeight() const;
protected:
	void mousePressEvent  (QMouseEvent* e) override;
	void mouseMoveEvent   (QMouseEvent* e) override;
	void mouseReleaseEvent(QMouseEvent* e) override;
	void keyPressEvent    (QKeyEvent*   e) override;
	void keyReleaseEvent  (QKeyEvent*   e) override;
private:
	QPoint mousePos;
	PanStatus panStatus = PanStatus::None;

	bool updatePanStatus(bool modifierPressed, MausButton mouseButtonPressed);

public slots:
	void scrollTo(int x, int y);
};

#endif // SCROLLAREAPAN_H
