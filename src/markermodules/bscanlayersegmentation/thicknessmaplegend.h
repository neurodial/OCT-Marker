#ifndef THICKNESSMAPLEGEND_H
#define THICKNESSMAPLEGEND_H

#include <QWidget>
#include<QList>


class QLabel;
class Colormap;

class ThicknessmapLegend : public QWidget
{
	QPixmap legend;

	int broderH = 5;
	const int broder         = 5;
	const int legendBarWidth = 30;
	int labelsMaxWidth  = 10;
	int labelsMaxHeight = 10;
	const int distanceBarLabel = 10;

	int value2HeightPos(double value, int height);

	struct BarLabel
	{
		BarLabel(double value, QWidget* parent, int xpos);

		void setHPos(int);

		QLabel* label;
		double value;
	};

	Colormap* colormap;

	void adjustLabel(BarLabel& label, int height);
	void updateLabelsWidth();

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
