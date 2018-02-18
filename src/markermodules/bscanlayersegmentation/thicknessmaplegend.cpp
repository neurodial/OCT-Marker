#include "thicknessmaplegend.h"


#include<QPaintEvent>
#include<QPainter>
#include<QImage>
#include<QLabel>

#include "colormaphsv.h"



ThicknessmapLegend::BarLabel::BarLabel(double value, QWidget* parent, int xpos)
: label(new QLabel(QString("%1").arg(value), parent))
, value(value)
{
	QSize s = label->minimumSizeHint();
	label->setGeometry(xpos, 0, s.width(), s.height());
}

void ThicknessmapLegend::BarLabel::setHPos(int pos)
{
	QRect geo = label->geometry();
	label->setGeometry(geo.x(), pos-geo.height()/2, geo.width(), geo.height());
}


ThicknessmapLegend::ThicknessmapLegend(QWidget* parent, Qt::WindowFlags f)
: QWidget(parent, f)
{
	const int labelXpos = distanceBarLabel + legendBarWidth + broder;
	thicknessLabels.push_back(BarLabel(0  , this, labelXpos));
	thicknessLabels.push_back(BarLabel(50 , this, labelXpos));
	thicknessLabels.push_back(BarLabel(100, this, labelXpos));
	thicknessLabels.push_back(BarLabel(200, this, labelXpos));
	thicknessLabels.push_back(BarLabel(300, this, labelXpos));
	thicknessLabels.push_back(BarLabel(400, this, labelXpos));
	thicknessLabels.push_back(BarLabel(500, this, labelXpos));
	updateLabelsWidth();
}

ThicknessmapLegend::~ThicknessmapLegend()
{
}

void ThicknessmapLegend::updateLabelsWidth()
{
	labelsMaxHeight = 0;
	labelsMaxWidth  = 0;
	for(BarLabel& label : thicknessLabels)
	{
		QSize s = label.label->minimumSizeHint();
		if(labelsMaxWidth < s.width())
			labelsMaxWidth = s.width();
		if(labelsMaxHeight < s.height())
			labelsMaxHeight = s.height();
	}
	broderH = labelsMaxHeight/2+broder;
}


void ThicknessmapLegend::adjustLabel(ThicknessmapLegend::BarLabel& label, int height)
{
	label.setHPos(value2HeightPos(label.value, height));
}


QSize ThicknessmapLegend::minimumSizeHint() const
{
	return QSize(broder*2 + legendBarWidth + distanceBarLabel + labelsMaxWidth, 200);
}

void ThicknessmapLegend::paintEvent(QPaintEvent* event)
{
	QPainter p(this);

	p.drawPixmap(broder, broderH, legend);

	int lineStartX = broder + legendBarWidth;
	int lineEndX   = lineStartX + distanceBarLabel/2;

	QPen pen(Qt::black);
	p.setPen(pen);

	for(BarLabel& label : thicknessLabels)
	{
// 		label.label->setFont();
		QRect geo = label.label->geometry();
		int h = geo.height()/2 + geo.y();
		p.drawLine(lineStartX, h, lineEndX, h);
	}
}

int ThicknessmapLegend::value2HeightPos(double value, int height)
{
	const int barHeight = height - 2*broderH;
	const double maxValue = colormap->getMaxValue();
	const double relValue = 1-value/maxValue;

	size();

	return static_cast<int>(relValue*barHeight) + broderH;
}

namespace
{
	void drawBlackLine(QRgb* line, int size)
	{
		for(int x = 0; x < size; ++x)
		{
			*line = Qt::black;
			++line;
		}
	}
}


void ThicknessmapLegend::resizeEvent(QResizeEvent* event)
{
	updateLegend(event->size());
}


void ThicknessmapLegend::updateLegend()
{
	updateLegend(size());
}

void ThicknessmapLegend::updateLegend(const QSize& wgSize)
{
	if(!colormap)
		return;

	const int legendBarHeight = wgSize.height()-broderH*2;
	QImage legendImage(legendBarWidth, legendBarHeight, QImage::Format_RGB32);
	double maxValue = colormap->getMaxValue();

	for(int i = 1; i<legendBarHeight-1; ++i)
	{
		const double pos = maxValue*(legendBarHeight - i)/legendBarHeight;
		uchar r, g, b;
		colormap->getColor(pos, r, g, b);
		QRgb rgb = qRgb(r, g, b);
		QRgb* line = reinterpret_cast<QRgb*>(legendImage.scanLine(i));
		*line = Qt::black;
		++line;
		for(int x = 2; x < legendBarWidth; ++x)
		{
			*line = rgb;
			++line;
		}
		*line = Qt::black;
		++line;
	}

	drawBlackLine(reinterpret_cast<QRgb*>(legendImage.scanLine(0                )), legendBarWidth);
	drawBlackLine(reinterpret_cast<QRgb*>(legendImage.scanLine(legendBarHeight-1)), legendBarWidth);


	legend = QPixmap::fromImage(legendImage);


	for(BarLabel& label : thicknessLabels)
		adjustLabel(label, wgSize.height());
}
