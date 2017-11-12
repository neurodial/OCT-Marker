#ifndef WIDGETOBJECTMARKER_H
#define WIDGETOBJECTMARKER_H


#include <QWidget>

class QAbstractButton;


class WidgetObjectMarker : public QWidget
{
	Q_OBJECT

	QAbstractButton* addObjectButton = nullptr;
public:
	explicit WidgetObjectMarker(QWidget* parent = nullptr);

signals:
	void addObjectStatusChangedSignal(bool v);

public slots:
	void addObjectStatusChangedSlot(bool v);

private slots:
	void addObjectStatusChangedSlotPrivte(bool v);
};

#endif // WIDGETOBJECTMARKER_H
