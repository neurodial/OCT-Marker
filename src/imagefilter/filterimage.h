#pragma once

#include <QObject>

namespace cv { class Mat; }

class FilterImage : public QObject
{
	Q_OBJECT
public:
	virtual void applyFilter(const cv::Mat& in, cv::Mat& out) const = 0;


signals:
	void parameterChanged();
};

