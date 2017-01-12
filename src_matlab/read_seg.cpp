#include "mex.h"

#include "helper/matlab_helper.h"
#include "helper/matlab_types.h"

#include <manager/octmarkerio.h>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/property_tree/json_parser.hpp>

namespace bpt = boost::property_tree;

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

	OctMarkerIO markerIO(&octmarkerTree);
	if(markerIO.loadDefaultMarker(filename))
	{
	}
	else
	{
		mexErrMsgIdAndTxt("MATLAB:mexcpp:nargin", "cant open marker file");

	}





}
