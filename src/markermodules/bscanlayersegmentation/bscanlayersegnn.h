#ifndef BSCANLAYERSEGNN_H
#define BSCANLAYERSEGNN_H

#include<string>

#ifdef ML_SUPPORT

class Callback;

namespace cv { class Mat; }
namespace FANN { class neural_net; }

class BscanLayerSegNN
{
public:
	void loadNN(const std::string& file);
	void saveNN(const std::string& file) const;
};

#endif

#endif // BSCANLAYERSEGNN_H
