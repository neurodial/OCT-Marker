#include "thicknessmaplegend.h"

#include<cmath>

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

ThicknessmapLegend::BarLabel::BarLabel(ThicknessmapLegend::BarLabel&& other)
: value(other.value)
{
	std::swap(label, other.label);
}

ThicknessmapLegend::BarLabel::~BarLabel()
{
	delete label;
}

ThicknessmapLegend::BarLabel& ThicknessmapLegend::BarLabel::operator=(ThicknessmapLegend::BarLabel&& other)
{
	value = other.value;
	std::swap(label, other.label);
	return *this;
}

void ThicknessmapLegend::BarLabel::setVisible(bool s)
{
	if(!label)
		return;
	label->setVisible(s);
}

bool ThicknessmapLegend::BarLabel::isVisible() const
{
	if(!label)
		return false;
	return label->isVisible();
}




void ThicknessmapLegend::BarLabel::setHPos(int pos)
{
	if(!label)
		return;
	QRect geo = label->geometry();
	label->setGeometry(geo.x(), pos-geo.height()/2, geo.width(), geo.height());
}

void ThicknessmapLegend::BarLabel::setValue(double v)
{
	if(!label)
		return;
	value = v;
	QString text = QString("%1").arg(value);
	label->setText(text);

	QSize s = label->minimumSizeHint();
	const QRect r = label->geometry();
	label->setGeometry(r.x(), 0, s.width(), s.height());
}



ThicknessmapLegend::ThicknessmapLegend(QWidget* parent, Qt::WindowFlags f)
: QWidget(parent, f)
{
	updateLabelXpos();
	thicknessLabels.push_back(BarLabel(0  , this, labelXpos));
// 	thicknessLabels.push_back(BarLabel(50 , this, labelXpos));
// 	thicknessLabels.push_back(BarLabel(100, this, labelXpos));
// 	thicknessLabels.push_back(BarLabel(200, this, labelXpos));
// 	thicknessLabels.push_back(BarLabel(300, this, labelXpos));
// 	thicknessLabels.push_back(BarLabel(400, this, labelXpos));
// 	thicknessLabels.push_back(BarLabel(500, this, labelXpos));
	updateLegendLabels();
}

ThicknessmapLegend::~ThicknessmapLegend()
{
}

void ThicknessmapLegend::updateLabelXpos()
{
	labelXpos = distanceBarLabel + legendBarWidth + broder;
}


void ThicknessmapLegend::calcLabelsSize(int& maxHeight, int& maxWidth)
{
	maxHeight = 0;
	maxWidth  = 0;
	for(BarLabel& label : thicknessLabels)
	{
		if(!label.isVisible())
			break;

		const QSize s = label.getLabel()->minimumSizeHint();

		int height = s.height();
		int width  = s.width();

		if(maxWidth < width)
			maxWidth = width;
		if(maxHeight < height)
			maxHeight = height;
	}
}

void ThicknessmapLegend::updateLabelsWidth()
{
	int maxHeight = 0;
	int maxWidth  = 0;
	calcLabelsSize(maxHeight, maxWidth);

	if(maxHeight == 0 || maxWidth == 0)
		return;

	labelsMaxHeight = maxHeight;
	labelsMaxWidth  = maxWidth ;

	broderH = labelsMaxHeight/2+broder;
	updateGeometry();
}


void ThicknessmapLegend::adjustLabel(ThicknessmapLegend::BarLabel& label, int height)
{
	label.setHPos(value2HeightPos(label.getValue(), height));
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
		if(!label.isVisible())
			break;

		QRect geo = label.getLabel()->geometry();
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

	updateLegendLabels();

	for(BarLabel& label : thicknessLabels)
		adjustLabel(label, wgSize.height()-1);
}


void ThicknessmapLegend::updateInsertLabel(std::size_t nr, double value)
{
	if(thicknessLabels.size() <= nr)
		thicknessLabels.push_back(BarLabel(value, this, labelXpos));
	else
	{
		thicknessLabels[nr].setValue(value);
		thicknessLabels[nr].setVisible(true);
	}

}



void ThicknessmapLegend::updateLegendLabels()
{
	if(!colormap)
		return;

	std::size_t actLabelNr = 1;

	const int labelDistance = 4;
	const int widgetHeight = height();

	if(widgetHeight == 0)
		return;

	const int maxLabels = widgetHeight/labelsMaxHeight/labelDistance;
	if(maxLabels > 0)
	{
		double maxValue = colormap->getMaxValue();
		double numDigits = std::floor(std::log10(maxValue));
		double magnitude = std::pow(10, numDigits);

		double highstSignDigit = std::floor(maxValue/magnitude);

		if(highstSignDigit == 1)
		{
			magnitude /= 10;
			highstSignDigit = std::floor(maxValue/magnitude);
		}

		int stepSize = static_cast<int>(highstSignDigit)/maxLabels;
		if(stepSize == 0)
			stepSize = 1;

		for(int i = 0; i <= highstSignDigit; i += stepSize)
		{
			updateInsertLabel(actLabelNr, i*magnitude);
			++actLabelNr;
		}
	}

	for(std::size_t i = actLabelNr; i < thicknessLabels.size(); ++i)
		thicknessLabels[i].setVisible(false);

	updateLabelsWidth();
}

