/*
 * Copyright (c) 2018 Kay Gawlik <kaydev@amarunet.de> <kay.gawlik@beuth-hochschule.de> <kay.gawlik@charite.de>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

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
