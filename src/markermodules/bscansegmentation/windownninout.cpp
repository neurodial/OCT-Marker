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

#include "windownninout.h"

#include <widgets/cvimagewidget.h>

#include <QWidget>
#include <QHBoxLayout>

#include <opencv/cv.hpp>

#include <iostream>

WindowNNInOut::WindowNNInOut()
: inoutNeuronsWidget(new QWidget())
, inputNeurons(new CVImageWidget(this))
, outputNeurons(new CVImageWidget(this))
{

	QHBoxLayout* layout = new QHBoxLayout;

	inputNeurons ->setScaleFactor(15);
	outputNeurons->setScaleFactor(15);

// 	inputNeurons ->setFloatGrayTransform(CVImageWidget::FloatGrayTransform::Fix);
// 	outputNeurons->setFloatGrayTransform(CVImageWidget::FloatGrayTransform::Fix);
//
// 	inputNeurons ->setGrayTransformValueA(255);
// 	inputNeurons ->setGrayTransformValueB(128);
// 	outputNeurons->setGrayTransformValueA(255);
// 	outputNeurons->setGrayTransformValueB(128);

	layout->addWidget(inputNeurons );
	layout->addWidget(outputNeurons);

	// Set layout in QWidget
	QWidget *window = new QWidget();
	window->setLayout(layout);

	// Set QWidget as the central layout of the main window
	setCentralWidget(window);

	setGeometry(0,0,100,100);
}


void WindowNNInOut::showInOutMat(const cv::Mat& in, const cv::Mat& out)
{
	std::cout << "In Channels: " << in.channels() << "\tgr: " << in.rows << " x " << in.cols << "\t Out Channels: " << out.channels() << "\tgr: " << out.rows << " x " << out.cols << std::endl;

	cv::Mat inImg, outImg;
	in .convertTo( inImg, cv::DataType<uint8_t>::type, 128, 0.5*255);
	out.convertTo(outImg, cv::DataType<uint8_t>::type, 128, 0.5*255);

	inputNeurons ->showImage(inImg );
	outputNeurons->showImage(outImg);
}

