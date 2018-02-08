#include "thicknessmaplegend.h"


#include<QPaintEvent>
#include<QPainter>
#include<QImage>
#include<QLabel>

#include "colormaphsv.h"


ThicknessmapLegend::BarLabel::BarLabel(double value, QWidget* parent)
: label(new QLabel(QString("%1").arg(value), parent))
, value(value)
{
	QSize s = label->minimumSizeHint();
	label->setGeometry(40, 0, s.width(), s.height());
}

void ThicknessmapLegend::BarLabel::setHPos(int pos)
{
	QRect geo = label->geometry();
	label->setGeometry(geo.x(), pos-geo.height()/2, geo.width(), geo.height());
}


ThicknessmapLegend::ThicknessmapLegend(QWidget* parent, Qt::WindowFlags f)
: QWidget(parent, f)
{
	thicknessLabels.push_back(BarLabel(0  , this));
	thicknessLabels.push_back(BarLabel(50, this));
	thicknessLabels.push_back(BarLabel(100, this));
	thicknessLabels.push_back(BarLabel(200, this));
	thicknessLabels.push_back(BarLabel(300, this));
	thicknessLabels.push_back(BarLabel(400, this));
	thicknessLabels.push_back(BarLabel(500, this));
}

ThicknessmapLegend::~ThicknessmapLegend()
{

}

void ThicknessmapLegend::adjustLabel(ThicknessmapLegend::BarLabel& label, int height)
{
	label.setHPos(value2HeightPos(label.value, height));
}


QSize ThicknessmapLegend::minimumSizeHint() const
{
	return QSize(50+broder*2, 200);
}

void ThicknessmapLegend::paintEvent(QPaintEvent* event)
{
	QPainter p(this);

	p.drawPixmap(broder, broder, legend);

}

int ThicknessmapLegend::value2HeightPos(double value, int height)
{
	ColormapHSV colormap;

	const int barHeight = height - 2*broder;
	const double maxValue = colormap.getMaxValue();
	const double relValue = 1-value/maxValue;

	return static_cast<int>(relValue*barHeight) + broder;
}


void ThicknessmapLegend::resizeEvent(QResizeEvent* event)
{
	QSize s = event->size();

	ColormapHSV colormap;

	const int legendBarHeight = s.height()-broder*2;
	const int legendBarWidth  = 30;

	QImage legendImage(legendBarWidth, legendBarHeight, QImage::Format_RGB32);

	double maxValue = colormap.getMaxValue();

	for(int i = 0; i<legendBarHeight; ++i)
	{
		const double pos = maxValue*(legendBarHeight - i)/legendBarHeight;
		uchar r, g, b;
		colormap.getColor(pos, r, g, b);
		QRgb rgb = qRgb(r, g, b);
		QRgb* line = reinterpret_cast<QRgb*>(legendImage.scanLine(i));
		for(int x = 0; x < legendBarWidth; ++x)
		{
			*line = rgb;
			++line;
		}
	}


	legend = QPixmap::fromImage(legendImage);


	for(BarLabel& label : thicknessLabels)
		adjustLabel(label, s.height());
}
