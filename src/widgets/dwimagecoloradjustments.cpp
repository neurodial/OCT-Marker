#include "dwimagecoloradjustments.h"

#include <QSlider>

#include <imagefilter/filtergammacontrastbrightness.h>
#include <QFormLayout>

DWImageColorAdjustments::DWImageColorAdjustments(QWidget* parent)
: QDockWidget(parent)
, imageFilter(new FilterGammaContrastBrightness)
{
	setWindowTitle(tr("image color adjustments"));

	QWidget* widget  = new QWidget(this);
	gammaSlider      = new QSlider(Qt::Horizontal, this);
	contrastSlider   = new QSlider(Qt::Horizontal, this);
	brightnessSlider = new QSlider(Qt::Horizontal, this);

	connect(gammaSlider     , &QSlider::valueChanged, this, &DWImageColorAdjustments::parameterChanged);
	connect(contrastSlider  , &QSlider::valueChanged, this, &DWImageColorAdjustments::parameterChanged);
	connect(brightnessSlider, &QSlider::valueChanged, this, &DWImageColorAdjustments::parameterChanged);


	gammaSlider     ->setMinimum(   0);
	gammaSlider     ->setMaximum( 200);
	gammaSlider     ->setValue  ( 100);
	contrastSlider  ->setMinimum(   0);
	contrastSlider  ->setMaximum( 200);
	contrastSlider  ->setValue  ( 100);
	brightnessSlider->setMinimum(-100);
	brightnessSlider->setMaximum( 100);

	gammaSlider     ->setTickPosition(QSlider::TicksBelow);
	gammaSlider     ->setTickInterval(20);
	contrastSlider  ->setTickPosition(QSlider::TicksBelow);
	contrastSlider  ->setTickInterval(20);
	brightnessSlider->setTickPosition(QSlider::TicksBelow);
	brightnessSlider->setTickInterval(20);

	QFormLayout *formLayout = new QFormLayout;
	formLayout->addRow(tr("Gamma:"     ), gammaSlider     );
	formLayout->addRow(tr("Contrast:"  ), contrastSlider  );
	formLayout->addRow(tr("Brightness:"), brightnessSlider);
	widget->setLayout(formLayout);

	parameterChanged();
	setWidget(widget);
}

DWImageColorAdjustments::~DWImageColorAdjustments()
{
	delete imageFilter;
}


const FilterImage* DWImageColorAdjustments::getImageFilter() const
{
	return imageFilter;
}


void DWImageColorAdjustments::parameterChanged()
{
	FilterGammaContrastBrightness::Parameter para;

	para.gamma      =  static_cast<double>(gammaSlider     ->value())/100.;
	para.contrast   =  static_cast<double>(contrastSlider  ->value())/100.;
	para.brightness =  static_cast<double>(brightnessSlider->value())/100.;

	imageFilter->setParameter(para);
}
