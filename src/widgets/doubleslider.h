#pragma once

#include<QSlider>


class DoubleSlider : public QSlider
{
	Q_OBJECT

	const double factor;
public:
	DoubleSlider(int factor, QWidget* parent)
	: QSlider(parent)
	, factor(factor)
	{
		connect(this, &QSlider::valueChanged, this, &DoubleSlider::vChanged);
	}

	DoubleSlider(Qt::Orientation orientation, int factor, QWidget* parent)
	: QSlider(orientation, parent)
	, factor(factor)
	{
		connect(this, &QSlider::valueChanged, this, &DoubleSlider::vChanged);
	}


	void setRange(double min, double max)
	{
		QSlider::setRange(static_cast<int>(min*factor)
		                , static_cast<int>(max*factor));
	}

private slots:
	void vChanged(int v) { emit(valueChanged(v/factor)); }

public slots:
	void setValue(double v) { QSlider::setValue(static_cast<int>(v*factor)); }

signals:
	void valueChanged(double);

};
