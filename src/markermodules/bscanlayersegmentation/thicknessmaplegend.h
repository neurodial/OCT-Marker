#ifndef THICKNESSMAPLEGEND_H
#define THICKNESSMAPLEGEND_H

#include <QWidget>
#include<QList>


class QLabel;

class ThicknessmapLegend : public QWidget
{
	QPixmap legend;

	int broder = 5;

	int value2HeightPos(double value, int height);

	struct BarLabel
	{
		BarLabel(double value, QWidget* parent);

		void setHPos(int);

		QLabel* label;
		double value;
	};


	void adjustLabel(BarLabel& label, int height);

public:
	ThicknessmapLegend(QWidget* parent = Q_NULLPTR, Qt::WindowFlags f = Qt::WindowFlags());
	~ThicknessmapLegend();


	virtual QSize minimumSizeHint() const;

protected:
	virtual void paintEvent(QPaintEvent* event);
	virtual void resizeEvent(QResizeEvent* event);


	QList<BarLabel> thicknessLabels;
};

#endif // THICKNESSMAPLEGEND_H
