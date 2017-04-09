#include "mex.h"

#include "helper/matlab_helper.h"
#include "helper/matlab_types.h"

#include <manager/octmarkerio.h>
#include <data_structure/simplematcompress.h>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/property_tree/json_parser.hpp>

#include <boost/archive/text_iarchive.hpp>

#include <iostream>
#include <sstream>

namespace bpt = boost::property_tree;




bool extractMatCompress(const bpt::ptree& nodeBscan, SimpleMatCompress& compressedMat)
{
	boost::optional<const bpt::ptree&> matCompressNodeOptional = nodeBscan.get_child_optional("matCompress");
	if(!matCompressNodeOptional)
		return false;

	std::string matCompressStr = matCompressNodeOptional->get_value<std::string>(std::string());

	if(matCompressStr.size() < 2)
		return false;

	std::stringstream ioa(matCompressStr);
	boost::archive::text_iarchive oa(ioa);
	oa >> compressedMat;
	return true;
}

void mexFunction(int            nlhs  ,
                 mxArray*       plhs[],
                 int            nrhs  ,
                 const mxArray* prhs[]
                 )
{
	/* Check for proper number of arguments */

	if(nrhs != 1)
	{
		mexErrMsgIdAndTxt("MATLAB:mexcpp:nargin", "MEXCPP requires 1 input arguments");
		return;
	}
	if(nlhs > 1)
	{
		mexErrMsgIdAndTxt("MATLAB:mexcpp:nargout", "MEXCPP requires only one output argument.");
		return;
	}

// 	mexPrintf("Read segmentation\n");

	if(!mxIsChar(prhs[0]))
	{
		mexErrMsgIdAndTxt("MATLAB:mexcpp:nargin", "requires filename");
		return;
	}
	// Filename
	mxChar* fnPtr = (mxChar*) mxGetPr(prhs[0]);
	std::size_t fnLength = mxGetN(prhs[0]);
	std::string filename(fnPtr, fnPtr+fnLength);


	bpt::ptree octmarkerTree;
	SimpleMatCompress compressedMat;

	OctMarkerIO markerIO(&octmarkerTree);
	if(!markerIO.loadDefaultMarker(filename))
	{
		mexErrMsgIdAndTxt("MATLAB:mexcpp:nargin", "can't open marker file for %s", filename.c_str());
		return;
	}

	mxArray* resultMat = nullptr;

	try
	{
		bpt::ptree& nodeSeries = octmarkerTree.get_child("Patient.Study.Series"  );
		bpt::ptree& nodeILM    = nodeSeries   .get_child("SegmentationMarker.ILM");

		mwSize numBscans = 0;
		mwSize rows = 0;
		mwSize cols = 0;

		// count BScans
		for(const std::pair<const std::string, bpt::ptree>& nodeBscanPair : nodeILM)
		{
			if(nodeBscanPair.first != "BScan")
				continue;

			if(!extractMatCompress(nodeBscanPair.second, compressedMat))
				continue;

			if(numBscans == 0)
			{
				rows = compressedMat.getRows();
				cols = compressedMat.getCols();
			}
			else if(rows != compressedMat.getRows() || cols != compressedMat.getCols())
				continue;

			++numBscans;
		}

		// create output mat
		const mwSize dims[] = {cols, rows, numBscans};
		const mwSize dimNum = sizeof(dims)/sizeof(dims[0]);
		resultMat = mxCreateNumericArray(dimNum, dims, MatlabType<uint8_t>::classID, mxREAL);
		uint8_t* dataPtr = reinterpret_cast<uint8_t*>(mxGetPr(resultMat));

		// fill output mat
		for(const std::pair<const std::string, bpt::ptree>& nodeBscanPair : nodeILM)
		{
			if(nodeBscanPair.first != "BScan")
				continue;

			if(!extractMatCompress(nodeBscanPair.second, compressedMat))
				continue;

			if(rows != compressedMat.getRows() || cols != compressedMat.getCols())
				continue;

			compressedMat.writeToMat(dataPtr, rows, cols);
			dataPtr += rows*cols;
		}


	}
	catch(...)
	{
		mexErrMsgIdAndTxt("MATLAB:mexcpp:nargin", "Error while reading file");
		return;
	}

	plhs[0] = resultMat;

}
