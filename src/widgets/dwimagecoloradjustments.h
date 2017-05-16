#ifndef DWIMAGECONTRAST_H
#define DWIMAGECONTRAST_H

#include<QDockWidget>

class QSlider;
class FilterImage;
class FilterGammaContrastBrightness;

class DWImageColorAdjustments : public QDockWidget
{
	Q_OBJECT

	FilterGammaContrastBrightness* imageFilter;

	QSlider* gammaSlider      = nullptr;
	QSlider* contrastSlider   = nullptr;
	QSlider* brightnessSlider = nullptr;

public:

	explicit DWImageColorAdjustments(QWidget* parent = nullptr);
	virtual ~DWImageColorAdjustments();

	const FilterImage* getImageFilter() const;

private slots:
	void parameterChanged();
};

#endif // DWIMAGECONTRAST_H
