#ifndef PTREEHELPER_H
#define PTREEHELPER_H


#include <boost/property_tree/ptree_fwd.hpp>
#include <string>

class PTreeHelper
{
public:
	static boost::property_tree::ptree& get_put      (boost::property_tree::ptree& tree, const std::string& path);
	static boost::property_tree::ptree& getNodeWithId(boost::property_tree::ptree& tree, const std::string& searchNode, int id);

	static void putNotEmpty(boost::property_tree::ptree& tree, const std::string& path, const std::string& value);
};

#endif // PTREEHELPER_H
