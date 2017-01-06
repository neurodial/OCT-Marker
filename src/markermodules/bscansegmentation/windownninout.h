/*
 * <one line to give the program's name and a brief idea of what it does.>
 * Copyright (C) 2017  Kay Gawlik <kaydev@amarunet.de>
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

#ifndef WINDOWNNINOUT_H
#define WINDOWNNINOUT_H

#include <QMainWindow>

class QWidget;
class CVImageWidget;

namespace cv
{
	class Mat;
}

class WindowNNInOut : public QMainWindow
{
	QWidget* inoutNeuronsWidget;
	CVImageWidget* inputNeurons;
	CVImageWidget* outputNeurons;
public:
	WindowNNInOut();

	void showInOutMat(const cv::Mat& in, const cv::Mat& out);
};

#endif // WINDOWNNINOUT_H
